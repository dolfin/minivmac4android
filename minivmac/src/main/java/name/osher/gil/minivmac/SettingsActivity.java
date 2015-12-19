package name.osher.gil.minivmac;

import android.os.Bundle;
import android.preference.*;
import android.preference.Preference.OnPreferenceClickListener;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;

public class SettingsActivity extends PreferenceActivity {
	public static final String KEY_PREF_SCALE = "pref_scale";
	public static final String KEY_PREF_SCROLL = "pref_scroll";
	public static final String KEY_PREF_RESET = "pref_reset";
	public static final String KEY_PREF_INTERRUPT = "pref_interrupt";
	public static final String KEY_PREF_ABOUT = "pref_about";

	public static final int RESULT_RESET = 10;
	public static final int RESULT_INTERRUPT = 20;
	public static final int RESULT_ABOUT = 30;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);
        
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
}
