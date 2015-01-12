package name.osher.gil.minivmac;

import name.osher.gil.minivmac.R;
import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.view.View;
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
        findViewById(R.id.versionText).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                setBuildDisplay(!showBuild);
            }
        });
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
