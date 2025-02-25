package name.osher.gil.minivmac;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.TypedArray;
import android.net.Uri;
import android.os.Bundle;

import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;

import java.util.Objects;

public class SettingsFragment extends PreferenceFragmentCompat {
	private static final String TAG = "minivmac.SettingsFrag";

	public static final String KEY_PREF_MACHINE = "pref_machine";
	public static final String KEY_PREF_ROM = "pref_rom";
	public static final String KEY_PREF_ROM_FILE = "pref_rom_file";
	public static final String KEY_PREF_ROM_CHECKSUM = "pref_rom_checksum";
	public static final String KEY_PREF_DISK_MANAGER = "pref_disk_manager";
	public static final String KEY_PREF_KEYBOARDS = "pref_keyboards";
	public static final String KEY_PREF_MOUSE = "pref_mouse";
	public static final String KEY_PREF_SCALE = "pref_scale";
	public static final String KEY_PREF_SCROLL = "pref_scroll";
	public static final String KEY_PREF_SPEED = "pref_speed";
	public static final String KEY_PREF_RESET = "pref_reset";
	public static final String KEY_PREF_INTERRUPT = "pref_interrupt";
	public static final String KEY_PREF_WEBSITE_START = "pref_website_start";
	public static final String KEY_PREF_WEBSITE = "pref_website";
	public static final String KEY_PREF_ABOUT = "pref_about";

	public static final int RESULT_RESET = 10;
	public static final int RESULT_INTERRUPT = 20;
	public static final int RESULT_ABOUT = 30;

	private static final String MAC_II_ROM_PREFIX = "MacII";

	private SharedPreferences _preferences;

	ActivityResultLauncher<String> _getRom = registerForActivityResult(new ActivityResultContracts.GetContent(),
			new ActivityResultCallback<Uri>() {
				@Override
				public void onActivityResult(Uri uri) {
					RomManager romManager = new RomManager();
					romManager.loadRom(requireContext(), uri, () -> {
						Preference pref_rom = findPreference(KEY_PREF_ROM);
						if (pref_rom != null) {
							pref_rom.setSummary(romManager.getRomName());
							SharedPreferences.Editor edit = _preferences.edit();
							edit.putString(KEY_PREF_ROM, romManager.getRomName());
							edit.putString(KEY_PREF_ROM_FILE, romManager.getRomFileName());
							edit.putLong(KEY_PREF_ROM_CHECKSUM, romManager.getRomChecksum());
							edit.apply();
						}

						// Update machine based on ROM file
						ListPreference pref_machine = findPreference( KEY_PREF_MACHINE );
						if (pref_machine != null) {
							String[] romNames = getResources().getStringArray(R.array.machine_rom_names);
							String[] machineValues = getResources().getStringArray(R.array.machine_values);
							for (int i = 0; i < romNames.length; i++) {
								if (romNames[i].equals(romManager.getRomFileName())) {
									pref_machine.setValue(machineValues[i]);
									pref_machine.setSummary(getMachineString(machineValues[i]));
									break;
								}
							}
						}
					});
				}
			});

	@Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
		setPreferencesFromResource(R.xml.settings, rootKey);

		_preferences = getPreferenceManager().getSharedPreferences();
		Context context = getPreferenceManager().getContext();

		ListPreference pref_machine = findPreference( KEY_PREF_MACHINE );
		if (pref_machine != null) {
			String machineName = _preferences.getString(KEY_PREF_MACHINE, getDefaultMachine());
			pref_machine.setValue(machineName);
			pref_machine.setSummary(getMachineString(machineName));

			pref_machine.setOnPreferenceChangeListener((preference, newValue) -> {
				new AlertDialog.Builder(context)
						.setTitle("Confirm Change")
						.setMessage("Are you sure you want to change the machine? This will restart the emulator, and you will have to provide a new ROM file.")
						.setPositiveButton(android.R.string.yes, (dialog, which) -> {
							String moduleName = newValue.toString();
							SharedPreferences.Editor edit = _preferences.edit();
							edit.putString(KEY_PREF_MACHINE, moduleName);
							edit.apply();
							pref_machine.setSummary(getMachineString(moduleName));
						})
						.setNegativeButton(android.R.string.no, null)
						.show();
				return false;
			});
		}

		String romFile = _preferences.getString(KEY_PREF_ROM_FILE, getResources().getString(R.string.defaultRomFileName));
		String romName = _preferences.getString(KEY_PREF_ROM, getString(R.string.pref_rom_summ));
		Preference pref_rom = findPreference(KEY_PREF_ROM);
		if (pref_rom != null) {
			pref_rom.setOnPreferenceClickListener(preference -> {
				_getRom.launch("application/octet-stream");
				return true;
			});
			pref_rom.setSummary(romName);
		}

		ListPreference pref_keyboards = findPreference( KEY_PREF_KEYBOARDS );
		if (pref_keyboards != null) {
			pref_keyboards.setSummary(getKbdString(pref_keyboards.getValue()));
			pref_keyboards.setOnPreferenceChangeListener((preference, newValue) -> {
				//Set KBD
				pref_keyboards.setSummary(getKbdString(newValue.toString()));
				return true;
			});
		}

		Preference pref_disk_manager = findPreference( KEY_PREF_DISK_MANAGER );
		if (pref_disk_manager != null) {
			pref_disk_manager.setOnPreferenceClickListener(preference -> {
				Intent i = new Intent(context, DiskManagerActivity.class);
				startActivity(i);
				return true;
			});
		}

		Preference pref_website_start = findPreference( KEY_PREF_WEBSITE_START );
		if (pref_website_start != null) {
			pref_website_start.setOnPreferenceClickListener(preference -> {
				Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(getString(R.string.url_start)));
				startActivity(i);
				return true;
			});
		}

		Preference pref_website = findPreference( KEY_PREF_WEBSITE );
		if (pref_website != null) {
			pref_website.setOnPreferenceClickListener(preference -> {
				Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(getString(R.string.url_homepage)));
				startActivity(i);
				return true;
			});
		}

        Preference pref_about = findPreference( KEY_PREF_ABOUT );
		if (pref_about != null) {
			pref_about.setOnPreferenceClickListener(preference -> {
				requireActivity().setResult(RESULT_ABOUT);
				requireActivity().finish();
				return true;
			});
		}

		Preference pref_reset = findPreference( KEY_PREF_RESET );
		if (pref_reset != null) {
			pref_reset.setEnabled(!romFile.startsWith(MAC_II_ROM_PREFIX) && Core.isInitialized());
			pref_reset.setOnPreferenceClickListener(preference -> {
				requireActivity().setResult(RESULT_RESET);
				requireActivity().finish();
				return true;
			});
		}

		Preference pref_interrupt = findPreference( KEY_PREF_INTERRUPT );
		if (pref_interrupt != null) {
			pref_interrupt.setEnabled(!romFile.startsWith(MAC_II_ROM_PREFIX) && Core.isInitialized());
			pref_interrupt.setOnPreferenceClickListener(preference -> {
				requireActivity().setResult(RESULT_INTERRUPT);
				requireActivity().finish();
				return true;
			});
		}

		ListPreference pref_speed = findPreference( KEY_PREF_SPEED );
		if (pref_speed != null) {
			pref_speed.setEnabled(Core.isInitialized());
			if (Core.isInitialized()) {
				int speed = Core.getSpeed();
				pref_speed.setValue(String.valueOf(speed));
				pref_speed.setSummary(getSpeedString(speed));
			}
			pref_speed.setOnPreferenceChangeListener((preference, newValue) -> {
				int speed = Integer.parseInt(newValue.toString());
				Core.setSpeed(speed);
				pref_speed.setSummary(getSpeedString(speed));
				return true;
			});
		}
    }

	private String getKbdString(String kbd) {
		String summary = "";
		try (TypedArray kbd_ent = requireContext().getResources().obtainTypedArray(R.array.kbd_entries)) {
			try (TypedArray kbd_val = requireContext().getResources().obtainTypedArray(R.array.kbd_values)) {
				for (int i = 0; i < kbd_val.length(); i++) {
					String val = kbd_val.getString(i);
					if (Objects.equals(kbd, val)) {
						summary = kbd_ent.getString(i);
					}
				}
				return summary;
			}
		}
	}
	private String getSpeedString(int speed) {
		String summary = "";
		try (TypedArray speeds_ent = requireContext().getResources().obtainTypedArray(R.array.speed_entries)) {
			try (TypedArray speeds_val = requireContext().getResources().obtainTypedArray(R.array.speed_values)) {
				for (int i = 0; i < speeds_val.length(); i++) {
					int val = speeds_val.getInt(i, -1);
					if (val == speed) {
						summary = speeds_ent.getString(i);
					}
				}
				return summary;
			}
		}
	}

	private String getMachineString(String machine) {
		String summary = "";
		try (TypedArray machines_ent = requireContext().getResources().obtainTypedArray(R.array.machine_entries)) {
			try (TypedArray machines_val = requireContext().getResources().obtainTypedArray(R.array.machine_values)) {
				for (int i = 0; i < machines_val.length(); i++) {
					String val = machines_val.getString(i);
					if (Objects.equals(machine, val)) {
						summary = machines_ent.getString(i);
					}
				}
				return summary;
			}
		}
	}

	private String getDefaultMachine() {
		return getResources().getString(R.string.defaultModuleName);
	}
}
