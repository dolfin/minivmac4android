package name.osher.gil.minivmac;

import static android.widget.AdapterView.INVALID_POSITION;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.AbsListView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class DiskManagerActivity extends Activity {
    private final static int ACTIVITY_SELECT_DISK = 301;

    private ArrayAdapter<DiskImage> arrayAdapter;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.disk_manager);
        ListView list = (ListView) findViewById(R.id.disksList);
        Button add = (Button) findViewById(R.id.add);
        Button remove = (Button) findViewById(R.id.remove);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);

        toolbar.setOnClickListener(v -> finish());

        list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
        arrayAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_single_choice);
        list.setAdapter(arrayAdapter);

        refreshDisksList();

        add.setOnClickListener(v -> showOpenFileDialog());

        remove.setOnClickListener(v -> removeDiskDialog(list.getCheckedItemPosition()));
	}

    private void refreshDisksList() {
        // Initializing a new String Array
        File[] disks = FileManager.getInstance().getAvailableDisks();

        ArrayList<DiskImage> disks_list = new ArrayList<>();
        for (File disk : disks) {
            DiskImage di = new DiskImage(disk, disk.getName());
            disks_list.add(di);
        }

        arrayAdapter.clear();
        arrayAdapter.addAll(disks_list);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == ACTIVITY_SELECT_DISK) {
            if (resultCode == RESULT_OK && data != null) {
                Uri contentUri = data.getData();
                Log.w("DiskManagerActivity", contentUri.toString());

                InputStream diskFile = null;
                try {
                    diskFile = this.getContentResolver().openInputStream(contentUri);
                } catch (FileNotFoundException ex) {
                    // Unable to open Disk file.
                    return;
                }
                String diskName = FileManager.getInstance().getFileName(this, contentUri);
                File dst = FileManager.getInstance().getDataFile(diskName);
                try {
                    FileManager.getInstance().copy(diskFile, dst);
                } catch (IOException ex) {
                    // Unable to copy Disk
                    return;
                }

                refreshDisksList();
            }
        }
    }

    private void showOpenFileDialog() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("application/octet-stream");

        startActivityForResult(intent, ACTIVITY_SELECT_DISK);
    }

    private void removeDiskDialog(int selectedDiskImage) {
        android.app.AlertDialog.Builder alert = new android.app.AlertDialog.Builder(this);

	    if (selectedDiskImage != INVALID_POSITION) {
            DiskImage di = (DiskImage)arrayAdapter.getItem(selectedDiskImage);

            alert.setMessage(String.format(getString(R.string.removeDiskWarning), di.toString()));
            alert.setCancelable(true);
            alert.setPositiveButton(R.string.btn_yes, (dialog, which) -> removeDisk(di));
            alert.setNegativeButton(R.string.btn_no, null);
        } else {
            alert.setMessage(R.string.noDiskSelected);
            alert.setCancelable(true);
            alert.setNeutralButton(R.string.btn_ok, null);
        }

        AlertDialog d = alert.create();
        d.show();
    }

    private void removeDisk(DiskImage diskImage) {
        try {
            FileManager.getInstance().delete(diskImage.getFile());
        } catch (Exception ex) {
            android.app.AlertDialog.Builder alert = new android.app.AlertDialog.Builder(this);
            alert.setMessage(String.format(getString(R.string.cantRemoveDisk), diskImage.toString()));
            alert.setCancelable(true);
            alert.setNeutralButton(R.string.btn_ok, null);
            AlertDialog d = alert.create();
            d.show();
        }

        refreshDisksList();
    }

    public class DiskImage {
	    private final File _file;
	    private final String _name;

	    public DiskImage(File file, String name) {
	        _file = file;
	        _name = name;
        }

        public File getFile() { return _file; }
        public String getName() { return _name; }

        @NonNull
        @Override
        public String toString() { return getName(); }
    }
}
