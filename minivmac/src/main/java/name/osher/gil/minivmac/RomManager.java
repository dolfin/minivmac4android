package name.osher.gil.minivmac;

import android.app.ProgressDialog;
import android.content.Context;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class RomManager {
    private static final String TAG = "minivmac.RomManager";

    private static final int ROM_SIZE_MAC_PLUS = 0x20000;
    private static final int ROM_SIZE_MAC_II   = 0x40000;
    private static final String UNKNOWN_ROM = "Unknown ROM";
    private static final String GENERIC_ROM = "vMac.ROM";
    public static final long INVALID_CHECKSUM = -1;
    private static final Map<Integer, String> _romVersions;
    static {
        Map<Integer, String> aMap = new HashMap<>();
        aMap.put(0x28BA61CE, "Macintosh 128");
        aMap.put(0x28BA4E50, "Macintosh 512K");
        aMap.put(0x4D1EEEE1, "Macintosh Plus v1");
        aMap.put(0x4D1EEAE1, "Macintosh Plus v2");
        aMap.put(0x4D1F8172, "Macintosh Plus v3");
        aMap.put(0xB2E362A8, "Macintosh SE");
        aMap.put(0xA49F9914, "Macintosh Classic");
        aMap.put(0x97851DB6, "Macintosh II (800k v1)");
        aMap.put(0x9779D2C4, "Macintosh II (800k v2)");
        aMap.put(0x97221136, "Macintosh IIx");
        aMap.put(0xB306E171, "Macintosh SE FDHD");
        _romVersions = Collections.unmodifiableMap(aMap);
    }

    private static final Map<Integer, String> _romFileNames;
    static {
        Map<Integer, String> aMap = new HashMap<>();
        aMap.put(0x28BA61CE, "Mac128K.ROM");
        aMap.put(0x28BA4E50, "Mac128K.ROM");
        aMap.put(0x4D1EEEE1, "vMac.ROM");
        aMap.put(0x4D1EEAE1, "vMac.ROM");
        aMap.put(0x4D1F8172, "vMac.ROM");
        aMap.put(0xB2E362A8, "MacSE.ROM");
        aMap.put(0xA49F9914, "Classic.ROM");
        aMap.put(0x97851DB6, "MacII.ROM");
        aMap.put(0x9779D2C4, "MacII.ROM");
        aMap.put(0x97221136, "MacIIx.ROM");
        aMap.put(0xB306E171, "SEFDHD.ROM");
        _romFileNames = Collections.unmodifiableMap(aMap);
    }

    private String _romName = UNKNOWN_ROM;
    private String _romFileName = GENERIC_ROM;

    private long _romChecksum = INVALID_CHECKSUM;

    public String getRomName() { return _romName; }

    public String getRomFileName() { return _romFileName; }

    public long getRomChecksum() {
        return _romChecksum;
    }

    public void loadRom(Context context, Uri romUri, Runnable onSuccess) {
        if (romUri != null) {
            Log.i(TAG, String.format("Selected ROM file: %s", romUri));

            InputStream romFile;
            InputStream romFileForValidation;
            int fileSize;
            try {
                romFile = context.getContentResolver().openInputStream(romUri);
                romFileForValidation = context.getContentResolver().openInputStream(romUri);
                fileSize = romFile.available();
            } catch (IOException ex) {
                // Unable to open ROM file.
                Log.e(TAG, String.format("Unable to open ROM file: %s", romUri), ex);
                return;
            }

            ProgressDialog validateProgressDialog;
            validateProgressDialog = new ProgressDialog(context);
            validateProgressDialog.setCancelable(false);
            validateProgressDialog.setIndeterminate(false);
            validateProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            validateProgressDialog.setMax(RomManager.getRomSize());

            ProgressDialog copyProgressDialog;
            copyProgressDialog = new ProgressDialog(context);
            copyProgressDialog.setCancelable(false);
            copyProgressDialog.setIndeterminate(false);
            copyProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            copyProgressDialog.setMax(fileSize);

            ExecutorService executor = Executors.newSingleThreadExecutor();
            Handler handler = new Handler(Looper.getMainLooper());

            executor.execute(() -> {
                boolean romCopiedSuccessfully;

                // Validate ROM
                handler.post(() -> {
                    validateProgressDialog.setTitle(R.string.validatingRom);
                    validateProgressDialog.show();
                });
                long checksum = validateRom(romFileForValidation, progress
                        -> handler.post(() -> validateProgressDialog.setProgress(progress)));
                handler.post(validateProgressDialog::dismiss);

                // Copy ROM
                if (checksum != RomManager.INVALID_CHECKSUM) {
                    handler.post(() -> {
                        copyProgressDialog.setTitle(R.string.copyingRom);
                        copyProgressDialog.show();
                    });
                    romCopiedSuccessfully = copyRom(context, romFile, _romFileNames.get((int)checksum), progress
                            -> handler.post(() -> copyProgressDialog.setProgress(progress)));
                    handler.post(copyProgressDialog::dismiss);
                } else {
                    romCopiedSuccessfully = false;
                }

                handler.post(() -> {
                    if (romCopiedSuccessfully) {
                        onSuccess.run();
                    } else {
                        Toast.makeText(context, R.string.rom_load_error, Toast.LENGTH_LONG).show();
                    }
                });
            });
        } else {
            Log.i(TAG, "No file was selected.");
        }
    }

    private long validateRom(InputStream romFile, IProgressCallback callback) {
        long checksum;
        try {
            checksum = calculateChecksum(romFile, callback);
            if (checksum < 0) {
                Log.w(TAG, "Invalid ROM file");
                return INVALID_CHECKSUM;
            }
        } catch (IOException ex) {
            // Unable to validate ROM file.
            Log.e(TAG, "Unable to validate ROM file", ex);
            return INVALID_CHECKSUM;
        }

        String romName = _romVersions.get((int)checksum);
        if (romName == null) {
            _romName = UNKNOWN_ROM;
        } else {
            _romName = romName;
        }

        String romFileName = _romFileNames.get((int)checksum);
        if (romFileName == null) {
            _romFileName = GENERIC_ROM;
        } else {
            _romFileName = romFileName;
        }

        _romChecksum = checksum;

        return checksum;
    }

    private boolean copyRom(Context context, InputStream romFile, String dstFileName, IProgressCallback callback) {
        try {
            File dst = FileManager.getInstance().getRomFile(dstFileName);
            FileManager.getInstance().copy(romFile, dst, callback);
        } catch (IOException ex) {
            // Unable to copy ROM
            Log.e(TAG, "Unable to copy ROM file", ex);
            return false;
        }

        return true;
    }

    public static int getRomSize() {
        if (BuildConfig.FLAVOR.equals("macII")) {
            return ROM_SIZE_MAC_II;
        } else {
            return ROM_SIZE_MAC_PLUS;
        }
    }

    private long calculateChecksum(InputStream romFile, IProgressCallback callback) throws IOException {
        long i;
        long calculatedChecksum = 0;
        long signatureChecksum = 0;

        long totalBytesCopied = 0;

        for (i = 0 ; i < 4 ; i++) {
            signatureChecksum = signatureChecksum << 8;
            signatureChecksum += romFile.read();
            totalBytesCopied++;
        }

        int b1 = 0, b2 = 0;
        for (i = 0; (b1 = romFile.read()) != -1 && (b2 = romFile.read()) != -1 && totalBytesCopied < getRomSize(); i++) {
            calculatedChecksum += ((long) b1 << 8) | b2;

            totalBytesCopied += 2;
            int progress = (int) totalBytesCopied;

            if (callback != null) {
                callback.onProgressUpdated(progress);
            }

            if (signatureChecksum == calculatedChecksum) {
                return signatureChecksum;
            }
        }

        return -1;
    }
}
