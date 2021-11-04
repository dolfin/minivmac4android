package name.osher.gil.minivmac;

import android.content.Context;
import android.net.Uri;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class RomManager {
    private static final String TAG = "name.osher.gil.minivmac.RomManager";

    private static final int ROM_SIZE_MAC_PLUS = 0x20000;
    private static final int ROM_SIZE_MAC_II   = 0x40000;
    private static final String UNKNOWN_ROM = "Unknown ROM";
    private static final Map<Integer, String> _romVersions;
    static {
        Map<Integer, String> aMap = new HashMap<>();
        aMap.put(0x28BA61CE, "Macintosh 128");
        aMap.put(0x28BA4E50, "Macintosh 512K");
        aMap.put(0x4D1EEEE1, "Macintosh Plus v1");
        aMap.put(0x4D1EEAE1, "Macintosh Plus v2");
        aMap.put(0x4D1F8172, "Macintosh Plus v3");
        aMap.put(0xB2E362A8, "Macintosh SE");
        aMap.put(0x97851DB6, "Macintosh II (800k v1)");
        aMap.put(0x9779D2C4, "Macintosh II (800k v2)");
        aMap.put(0x97221136, "Macintosh IIx");
        aMap.put(0xB306E171, "Macintosh SE FDHD");
        _romVersions = Collections.unmodifiableMap(aMap);
    }

    private String _romName = UNKNOWN_ROM;

    public String getRomName() { return _romName; }

    public boolean loadRom(Context context, Uri romUri) {
        InputStream romFile;
        InputStream romFileForValidation;
        try {
            romFile = context.getContentResolver().openInputStream(romUri);
            romFileForValidation = context.getContentResolver().openInputStream(romUri);
        } catch (FileNotFoundException ex) {
            // Unable to open ROM file.
            Log.e(TAG, String.format("Unable to open ROM file: %s", romUri), ex);
            return false;
        }

        long checksum;
        try {
            checksum = validateRom(romFileForValidation);
            if (checksum < 0) {
                Log.w(TAG, String.format("Invalid ROM file: %s", romUri));
                return false;
            }
        } catch (IOException ex) {
            // Unable to validate ROM file.
            Log.e(TAG, String.format("Unable to validate ROM file: %s", romUri), ex);
            return false;
        }

        File dst = FileManager.getInstance().getRomFile(context.getString(R.string.romFileName));
        try {
            FileManager.getInstance().copy(romFile, dst);
        } catch (IOException ex) {
            // Unable to copy ROM
            Log.e(TAG, String.format("Unable to copy ROM file: %s", romUri), ex);
            return false;
        }

        String romName = _romVersions.get((int)checksum);
        if (romName == null) {
            _romName = UNKNOWN_ROM;
        } else {
            _romName = romName;
        }
        return true;
    }

    private int getRomSize() {
        if (BuildConfig.FLAVOR.equals("macII")) {
            return ROM_SIZE_MAC_II;
        } else {
            return ROM_SIZE_MAC_PLUS;
        }
    }

    private long validateRom(InputStream romFile) throws IOException {
        long i;
        long calculatedChecksum = 0;
        long signatureChecksum = 0;
        for (i = 0 ; i < 4 ; i++) {
            signatureChecksum = signatureChecksum << 8;
            signatureChecksum += romFile.read();
        }

        for (i = (getRomSize() - 4) >> 1; --i >= 0; ) {
            long b1 = romFile.read();
            long b2 = romFile.read();
            calculatedChecksum += (b1 << 8) | b2;
        }

        if (signatureChecksum == calculatedChecksum) {
            return signatureChecksum;
        } else {
            return -1;
        }
    }
}
