package name.osher.gil.minivmac;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.*;
import android.preference.Preference.OnPreferenceClickListener;

import androidx.appcompat.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.Toast;

public class SettingsActivity extends PreferenceActivity {
	private static final String TAG = "name.osher.gil.minivmac.SettingsActivity";

	public static final String KEY_PREF_ROM = "pref_rom";
	public static final String KEY_PREF_DISK_MANAGER = "pref_disk_manager";
	public static final String KEY_PREF_SCALE = "pref_scale";
	public static final String KEY_PREF_SCROLL = "pref_scroll";
	public static final String KEY_PREF_SPEED = "pref_speed";
	public static final String KEY_PREF_RESET = "pref_reset";
	public static final String KEY_PREF_INTERRUPT = "pref_interrupt";
	public static final String KEY_PREF_ABOUT = "pref_about";

	private final static int ACTIVITY_SELECT_ROM = 300;
	private final static int ACTIVITY_DISK_MANAGER = 204;

	public static final int RESULT_RESET = 10;
	public static final int RESULT_INTERRUPT = 20;
	public static final int RESULT_ABOUT = 30;

	private SharedPreferences _preferences;

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);

		_preferences = PreferenceManager.getDefaultSharedPreferences(this);

		Preference pref_rom = findPreference(KEY_PREF_ROM);
		pref_rom.setOnPreferenceClickListener(preference -> {
			Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
			intent.addCategory(Intent.CATEGORY_OPENABLE);
			intent.setType("application/octet-stream");

			startActivityForResult(intent, ACTIVITY_SELECT_ROM);
			return true;
		});
		String romName = _preferences.getString(KEY_PREF_ROM, getString(R.string.pref_rom_summ));
		pref_rom.setSummary(romName);

		Preference pref_disk_manager = findPreference( KEY_PREF_DISK_MANAGER );
		pref_disk_manager.setOnPreferenceClickListener(preference -> {
			Intent i = new Intent(SettingsActivity.this, DiskManagerActivity.class);
			startActivityForResult(i, ACTIVITY_DISK_MANAGER);
			return true;
		});

        Preference pref_about = findPreference( KEY_PREF_ABOUT );
        pref_about.setOnPreferenceClickListener(preference -> {
			setResult(RESULT_ABOUT);
			finish();
			return true;
		});

		Preference pref_reset = findPreference( KEY_PREF_RESET );
		pref_reset.setOnPreferenceClickListener(preference -> {
			setResult(RESULT_RESET);
			finish();
			return true;
		});

		Preference pref_interrupt = findPreference( KEY_PREF_INTERRUPT );
		pref_interrupt.setOnPreferenceClickListener(preference -> {
			setResult(RESULT_INTERRUPT);
			finish();
			return true;
		});

		ListPreference pref_speed = (ListPreference) findPreference( KEY_PREF_SPEED );
		//pref_speed.setValue(String.valueOf(Core.getSpeed()));
		pref_speed.setOnPreferenceChangeListener((preference, newValue) -> {
			Core.setSpeed(Integer.parseInt(newValue.toString()));
			return true;
		});
    }

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);

		LinearLayout root = (LinearLayout)findViewById(android.R.id.list).getParent().getParent().getParent();
		Toolbar bar = (Toolbar) LayoutInflater.from(this).inflate(R.layout.settings_toolbar, root, false);
		root.addView(bar, 0); // insert at top
		bar.setNavigationOnClickListener(v -> finish());
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		if (requestCode == ACTIVITY_SELECT_ROM) {
			if (resultCode == RESULT_OK && data != null) {
				Uri contentUri = data.getData();
				Log.i(TAG, String.format("Selected ROM file: %s", contentUri.toString()));
				Preference pref_rom = findPreference(KEY_PREF_ROM);

				RomManager romManager = new RomManager();
				if (romManager.loadRom(this, contentUri)) {
					pref_rom.setSummary(romManager.getRomName());
					SharedPreferences.Editor edit = _preferences.edit();
					edit.putString(KEY_PREF_ROM, romManager.getRomName());
					edit.apply();
				} else {
					Toast.makeText(this, R.string.rom_load_error, Toast.LENGTH_LONG).show();
				}
			}
		}
	}
}
