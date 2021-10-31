package name.osher.gil.minivmac;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.preference.*;
import android.preference.Preference.OnPreferenceClickListener;

import androidx.appcompat.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class SettingsActivity extends PreferenceActivity {
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
	public static final int RESULT_DISK_MANAGER = 40;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);

		Preference pref_rom = (Preference) findPreference(KEY_PREF_ROM);
		pref_rom.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
			  @Override
			  public boolean onPreferenceClick(Preference preference) {
				  Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
				  intent.addCategory(Intent.CATEGORY_OPENABLE);
				  intent.setType("application/octet-stream");

				  startActivityForResult(intent, ACTIVITY_SELECT_ROM);
				  return true;
			  }
		});

		Preference pref_disk_manager = findPreference( KEY_PREF_DISK_MANAGER );
		pref_disk_manager.setOnPreferenceClickListener( new OnPreferenceClickListener() {

			@Override
			public boolean onPreferenceClick(Preference preference) {
				Intent i = new Intent(SettingsActivity.this, DiskManagerActivity.class);
				startActivityForResult(i, ACTIVITY_DISK_MANAGER);
				//setResult(RESULT_DISK_MANAGER);
				//finish();
				return true;
			}
		} );

        Preference pref_about = findPreference( KEY_PREF_ABOUT );
        pref_about.setOnPreferenceClickListener( new OnPreferenceClickListener() {

			@Override
			public boolean onPreferenceClick(Preference preference) {
				setResult(RESULT_ABOUT);
				finish();
				return true;
			}
		} );

		Preference pref_reset = findPreference( KEY_PREF_RESET );
		pref_reset.setOnPreferenceClickListener( new OnPreferenceClickListener() {

			@Override
			public boolean onPreferenceClick(Preference preference) {
				setResult(RESULT_RESET);
				finish();
				return true;
			}
		} );

		Preference pref_interrupt = findPreference( KEY_PREF_INTERRUPT );
		pref_interrupt.setOnPreferenceClickListener( new OnPreferenceClickListener() {

			@Override
			public boolean onPreferenceClick(Preference preference) {
				setResult(RESULT_INTERRUPT);
				finish();
				return true;
			}
		} );

		ListPreference pref_speed = (ListPreference) findPreference( KEY_PREF_SPEED );
		//pref_speed.setValue(String.valueOf(Core.getSpeed()));
		pref_speed.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
			@Override
			public boolean onPreferenceChange(Preference preference, Object newValue) {
				Core.setSpeed(Integer.parseInt(newValue.toString()));
				return true;
			}
		});
    }

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);

		LinearLayout root = (LinearLayout)findViewById(android.R.id.list).getParent().getParent().getParent();
		Toolbar bar = (Toolbar) LayoutInflater.from(this).inflate(R.layout.settings_toolbar, root, false);
		root.addView(bar, 0); // insert at top
		bar.setNavigationOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				finish();
			}
		});
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		if (requestCode == ACTIVITY_SELECT_ROM) {
			if (resultCode == RESULT_OK && data != null) {
				Uri contentUri = data.getData();
				Log.w("SettingsActivity", contentUri.toString());
				Preference pref_rom = (Preference) findPreference(KEY_PREF_ROM);

				InputStream romFile = null;
				try {
					romFile = this.getContentResolver().openInputStream(contentUri);
				} catch (FileNotFoundException ex) {
					// Unable to open ROM file.
					return;
				}

				File dst = FileManager.getInstance().getDataFile(getString(R.string.romFileName));
				try {
					FileManager.getInstance().copy(romFile, dst);
				} catch (IOException ex) {
					// Unable to copy ROM
					return;
				}

				pref_rom.setSummary("Selected");
			}
		}
	}
}
