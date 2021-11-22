package name.osher.gil.minivmac;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatButton;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.fragment.app.Fragment;


public class WelcomeFragment extends Fragment {
    private static final String TAG = "minivmac.WelcomeFrag";

    ActivityResultLauncher<String> _getRom = registerForActivityResult(new ActivityResultContracts.GetContent(),
            uri -> {
                if (uri != null) {
                    Log.i(TAG, String.format("Selected ROM file: %s", uri.toString()));

                    RomManager romManager = new RomManager();
                    if (romManager.loadRom(requireContext(), uri)) {
                        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(getContext());
                        SharedPreferences.Editor edit = sharedPref.edit();
                        edit.putString(SettingsFragment.KEY_PREF_ROM, romManager.getRomName());
                        edit.apply();
                        ((MiniVMac)requireActivity()).showEmulator();
                    } else {
                        Toast.makeText(getContext(), R.string.rom_load_error, Toast.LENGTH_LONG).show();
                    }
                } else {
                    Log.i(TAG, "No file was selected.");
                }
            });

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        ViewGroup root = (ViewGroup) inflater.inflate(R.layout.welcome, container, false);
        AppCompatTextView welcome_title = root.findViewById(R.id.welcome_text);
        welcome_title.setText(String.format(getString(R.string.welcome_title), getString(R.string.app_name)));
        AppCompatTextView welcome_desc = root.findViewById(R.id.welcome_desc);
        welcome_desc.setText(String.format(getString(R.string.welcome_desc), getString(R.string.model)));
        AppCompatTextView migration_title = root.findViewById(R.id.migration_title);
        migration_title.setText(String.format(getString(R.string.migration_title), getString(R.string.app_name)));

        AppCompatButton browse = root.findViewById(R.id.browse);
        browse.setOnClickListener(v -> _getRom.launch("application/octet-stream"));

        return root;
    }
}
