package name.osher.gil.minivmac;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.SpannableString;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.widget.TextView;

/**
 * Created by dolfin on 16/12/2015.
 */
public class Utils {
    public static void showAlert(Context context, String msg, boolean end) {
        showAlert(context, msg, end, null);
    }

    public static void showAlert(Context context, String msg, boolean end, DialogInterface.OnClickListener listener) {
        final SpannableString s = new SpannableString(msg);
        Linkify.addLinks(s, Linkify.ALL);

        AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setMessage(s);
        alert.setCancelable(false);
        if (end) {
            alert.setNegativeButton(R.string.btn_quit, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface di, int i) {
                    System.exit(0);
                }
            });
        } else {
            alert.setNeutralButton(R.string.btn_ok, listener);
        }

        AlertDialog d = alert.create();
        d.show();

        // Make the textview clickable. Must be called after show()
        ((TextView)d.findViewById(android.R.id.message)).setMovementMethod(LinkMovementMethod.getInstance());
    }

    public static void showWarnMessage(Context context, String title, String msg, boolean end,
                                       DialogInterface.OnClickListener contListener) {
        AlertDialog.Builder alert = new AlertDialog.Builder(context);
        alert.setTitle(title);
        alert.setMessage(msg);
        alert.setNegativeButton(R.string.btn_quit, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface di, int i) {
                System.exit(0);
            }
        });
        if (!end) {
            alert.setPositiveButton(R.string.btn_continue, contListener);
        }

        alert.show();
    }
}
