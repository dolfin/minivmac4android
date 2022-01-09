package name.osher.gil.minivmac;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.text.SpannableString;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.widget.TextView;

import androidx.core.content.FileProvider;

import java.io.File;
import java.util.List;

/**
 * Created by dolfin on 16/12/2015.
 */
public class Utils {
    public static void showAlert(Context context, String msg, boolean end) {
        showAlert(context, null, msg, end, null);
    }

    public static void showAlert(Context context, String title, String msg, boolean end, DialogInterface.OnClickListener listener) {
        final SpannableString s = new SpannableString(msg);
        Linkify.addLinks(s, Linkify.ALL);

        AlertDialog.Builder alert = new AlertDialog.Builder(context);
        if (title != null) {
            alert.setTitle(title);
        }
        alert.setMessage(s);
        alert.setCancelable(false);
        if (end) {
            alert.setNegativeButton(R.string.btn_quit, (di, i) -> System.exit(0));
        } else {
            alert.setNeutralButton(R.string.btn_ok, listener);
        }

        AlertDialog d = alert.create();
        d.show();

        // Make the textview clickable. Must be called after show()
        ((TextView)d.findViewById(android.R.id.message)).setMovementMethod(LinkMovementMethod.getInstance());
    }

    public static void showShareDialog(Context context, File file, String name) {
        Uri uri = Uri.fromFile(file);
        Uri exportUri = FileProvider.getUriForFile(context, String.format("%s.provider", BuildConfig.APPLICATION_ID),
                file, name);
        Intent shareIntent = new Intent();
        shareIntent.setAction(Intent.ACTION_SEND);
        shareIntent.putExtra(Intent.EXTRA_STREAM, exportUri);
        shareIntent.setType(FileManager.getInstance().getMimeType(uri));

        Intent chooser = Intent.createChooser(shareIntent, context.getResources().getText(R.string.send_to));
        List<ResolveInfo> resInfoList = context.getPackageManager().queryIntentActivities(chooser, PackageManager.MATCH_DEFAULT_ONLY);
        for (ResolveInfo resolveInfo : resInfoList) {
            String packageName = resolveInfo.activityInfo.packageName;
            context.grantUriPermission(packageName, exportUri, Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }
        context.startActivity(chooser);
    }
}
