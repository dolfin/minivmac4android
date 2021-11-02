package name.osher.gil.minivmac;

import static android.widget.AdapterView.INVALID_POSITION;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.text.DecimalFormat;
import java.util.ArrayList;

public class DiskManagerActivity extends Activity {
    private final static int ACTIVITY_SELECT_DISK = 301;
    private final static int ACTIVITY_CREATE_DISK = 302;

    private DisksListAdapter _adapter;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.disk_manager);
        ListView list = (ListView) findViewById(R.id.disksList);
        Button newDisk = (Button) findViewById(R.id.newDisk);
        Button add = (Button) findViewById(R.id.add);
        Button remove = (Button) findViewById(R.id.remove);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);

        toolbar.setOnClickListener(v -> finish());

        list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
        _adapter = new DisksListAdapter(this);
        list.setAdapter(_adapter);
        list.setOnItemClickListener((parent, view, position, id) -> {
            _adapter.setSelectedIndex(position);
        });

        refreshDisksList();

        newDisk.setOnClickListener(v -> showNewDiskDialog());
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

        _adapter.clear();
        _adapter.addAll(disks_list);
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
                String diskName = FileManager.getInstance().getFileName(contentUri);
                File dst = FileManager.getInstance().getDisksFile(diskName);
                try {
                    FileManager.getInstance().copy(diskFile, dst);
                } catch (IOException ex) {
                    // Unable to copy Disk
                    return;
                }

                refreshDisksList();
            }
        } else if (requestCode == ACTIVITY_CREATE_DISK) {
            refreshDisksList();
        }
    }

    private void showNewDiskDialog() {
        Intent i = new Intent(DiskManagerActivity.this, CreateDisk.class);
        startActivityForResult(i, ACTIVITY_CREATE_DISK);
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
            DiskImage di = (DiskImage) _adapter.getItem(selectedDiskImage);

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
	    private final long _size;

	    public DiskImage(File file, String name) {
	        _file = file;
	        _name = name;
	        _size = file.length();
        }

        public File getFile() { return _file; }
        public String getName() { return _name; }
        public long getSize() { return _size; }
        public String getReadableSize() {
            DecimalFormat dform = new DecimalFormat("#,###.##");
	        long rs = _size;
	        if (rs < 1024) {
	            return String.format("%s bytes", dform.format(rs));
            } else {
	            rs /= 1024;
	            if (rs < 1024) {
	                return String.format("%s Kb", dform.format(rs));
                } else {
                    rs /= 1024;
                    return String.format("%s Mb", dform.format(rs));
                }
            }
        }

        @NonNull
        @Override
        public String toString() { return getName(); }
    }

    public class DisksListAdapter extends ArrayAdapter<DiskImage> {
        private int _selectedIndex = -1;

        public DisksListAdapter(Context context) {
            super(context, 0);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            // Get the data item for this position
            DiskImage di = getItem(position);
            // Check if an existing view is being reused, otherwise inflate the view
            if (convertView == null) {
                convertView = LayoutInflater.from(getContext()).inflate(R.layout.simple_list_item_2_single_choice, parent, false);
            }
            // Lookup view for data population
            TextView name = (TextView) convertView.findViewById(R.id.text1);
            TextView size = (TextView) convertView.findViewById(R.id.text2);
            RadioButton radioButton = (RadioButton) convertView.findViewById(R.id.radio);
            // Populate the data into the template view using the data object
            name.setText(di.getName());
            size.setText(di.getReadableSize());
            radioButton.setChecked(position == _selectedIndex);
            // Return the completed view to render on screen
            return convertView;
        }

        void setSelectedIndex(int index) {
            _selectedIndex = index;
            notifyDataSetChanged();
        }
    }
}
