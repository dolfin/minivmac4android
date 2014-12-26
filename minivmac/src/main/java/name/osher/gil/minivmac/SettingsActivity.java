package name.osher.gil.minivmac;

import android.os.Bundle;
import android.preference.*;
import android.preference.Preference.OnPreferenceClickListener;

public class SettingsActivity extends PreferenceActivity {
	public static final String KEY_PREF_SCALE = "pref_scale";
	public static final String KEY_PREF_SCROLL = "pref_scroll";
	public static final String KEY_PREF_RESET = "pref_reset";
	public static final String KEY_PREF_INTERRUPT = "pref_interrupt";
	public static final String KEY_PREF_ABOUT = "pref_about";
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);
        
        Preference pref_about = findPreference( KEY_PREF_ABOUT );
        pref_about.setOnPreferenceClickListener( new OnPreferenceClickListener() {
			
			@Override
			public boolean onPreferenceClick(Preference preference) {
				MiniVMac.getInstance().showAbout();
				finish();
				return true;
			}
		} );
		
		Preference pref_reset = findPreference( KEY_PREF_RESET );
		pref_reset.setOnPreferenceClickListener( new OnPreferenceClickListener() {
			
			@Override
			public boolean onPreferenceClick(Preference preference) {
				MiniVMac.getInstance().reset();
				finish();
				return true;
			}
		} );
		
		Preference pref_interrupt = findPreference( KEY_PREF_INTERRUPT );
		pref_interrupt.setOnPreferenceClickListener( new OnPreferenceClickListener() {
			
			@Override
			public boolean onPreferenceClick(Preference preference) {
				MiniVMac.getInstance().interrupt();
				finish();
				return true;
			}
		} );
    }
}
