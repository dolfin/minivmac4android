package name.osher.gil.minivmac;

import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.text.method.LinkMovementMethod;
import android.widget.TextView;

public class AboutDialog extends Dialog {
	private boolean showBuild = true;

	public AboutDialog(Context context) {
		super(context);
        initDialog();
	}

    private void initDialog() {
        setContentView(R.layout.about);
        setTitle(R.string.aboutTitle);

		TextView privacyPolicyText = (TextView) findViewById(R.id.privacyPolicyText);
		privacyPolicyText.setMovementMethod(LinkMovementMethod.getInstance());

        findViewById(R.id.versionText).setOnClickListener(view -> setBuildDisplay(!showBuild));
        setBuildDisplay(false);
    }

	private void setBuildDisplay (boolean showBuild) {
		Context ctx = getContext();
		TextView versionText = (TextView) findViewById(R.id.versionText);
		PackageInfo pi;
		try {
			pi = ctx.getPackageManager().getPackageInfo(ctx.getPackageName(), 0);
		} catch (NameNotFoundException e) {
        	return;
        }
		if (showBuild) {
			versionText.setText(ctx.getString(R.string.build) + " " + pi.versionCode);
		} else {
			versionText.setText(ctx.getString(R.string.version) + " " + pi.versionName);
		}
		this.showBuild = showBuild;
	}
}
