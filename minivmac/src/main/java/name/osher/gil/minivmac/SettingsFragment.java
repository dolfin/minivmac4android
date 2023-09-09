package name.osher.gil.minivmac;

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

public class SettingsFragment extends PreferenceFragmentCompat {
	private static final String TAG = "minivmac.SettingsFrag";

	public static final String KEY_PREF_ROM = "pref_rom";
	public static final String KEY_PREF_DISK_MANAGER = "pref_disk_manager";
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

	private SharedPreferences _preferences;

	ActivityResultLauncher<String> _getRom = registerForActivityResult(new ActivityResultContracts.GetContent(),
			new ActivityResultCallback<Uri>() {
				@Override
				public void onActivityResult(Uri uri) {
					RomManager romManager = new RomManager();
					romManager.loadRom(requireContext(), uri, () -> {
						Preference pref_rom = findPreference(KEY_PREF_ROM);
						pref_rom.setSummary(romManager.getRomName());
						SharedPreferences.Editor edit = _preferences.edit();
						edit.putString(KEY_PREF_ROM, romManager.getRomName());
						edit.apply();
					});
				}
			});

	@Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
		setPreferencesFromResource(R.xml.settings, rootKey);

		_preferences = getPreferenceManager().getSharedPreferences();
		Context context = getPreferenceManager().getContext();

		Preference pref_rom = findPreference(KEY_PREF_ROM);
		pref_rom.setOnPreferenceClickListener(preference -> {
			_getRom.launch("application/octet-stream");
			return true;
		});
		String romName = _preferences.getString(KEY_PREF_ROM, getString(R.string.pref_rom_summ));
		pref_rom.setSummary(romName);

		Preference pref_disk_manager = findPreference( KEY_PREF_DISK_MANAGER );
		pref_disk_manager.setOnPreferenceClickListener(preference -> {
			Intent i = new Intent(context, DiskManagerActivity.class);
			startActivity(i);
			return true;
		});

		Preference pref_website_start = findPreference( KEY_PREF_WEBSITE_START );
		pref_website_start.setOnPreferenceClickListener(preference -> {
			Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(getString(R.string.url_start)));
			startActivity(i);
			return true;
		});

		Preference pref_website = findPreference( KEY_PREF_WEBSITE );
		pref_website.setOnPreferenceClickListener(preference -> {
			Intent i = new Intent(Intent.ACTION_VIEW, Uri.parse(getString(R.string.url_homepage)));
			startActivity(i);
			return true;
		});

        Preference pref_about = findPreference( KEY_PREF_ABOUT );
        pref_about.setOnPreferenceClickListener(preference -> {
			requireActivity().setResult(RESULT_ABOUT);
			requireActivity().finish();
			return true;
		});

		Preference pref_reset = findPreference( KEY_PREF_RESET );
		pref_reset.setEnabled(BuildConfig.FLAVOR.equals("macPlus") && Core.isInitialized());
		pref_reset.setOnPreferenceClickListener(preference -> {
			requireActivity().setResult(RESULT_RESET);
			requireActivity().finish();
			return true;
		});

		Preference pref_interrupt = findPreference( KEY_PREF_INTERRUPT );
		pref_interrupt.setEnabled(BuildConfig.FLAVOR.equals("macPlus") && Core.isInitialized());
		pref_interrupt.setOnPreferenceClickListener(preference -> {
			requireActivity().setResult(RESULT_INTERRUPT);
			requireActivity().finish();
			return true;
		});

		ListPreference pref_speed = findPreference( KEY_PREF_SPEED );
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

	private String getSpeedString(int speed) {
		String summary = "";
		TypedArray speeds_ent = getContext().getResources().obtainTypedArray(R.array.speed_entries);
		TypedArray speeds_val = getContext().getResources().obtainTypedArray(R.array.speed_values);
		for (int i = 0 ; i < speeds_val.length() ; i++) {
			int val = speeds_val.getInt(i, -1);
			if (val == speed) {
				summary = speeds_ent.getString(i);
			}
		}
		speeds_ent.recycle();
		speeds_val.recycle();
		return summary;
	}
}
