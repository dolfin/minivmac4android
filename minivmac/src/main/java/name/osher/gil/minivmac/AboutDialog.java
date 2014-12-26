package name.osher.gil.minivmac;

import name.osher.gil.minivmac.R;
import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.view.View;
import android.widget.TextView;

public class AboutDialog extends Dialog implements View.OnClickListener {
	private boolean showBuild = true;

	public AboutDialog(Context context) {
		super(context);
        setContentView(R.layout.about); 
        setTitle("About Mini vMac");
        findViewById(R.id.buttonOK).setOnClickListener(this);
        findViewById(R.id.versionText).setOnClickListener(this);
        setBuildDisplay(false);
	}
	
	public void onClick(View v) {
		if (v == findViewById(R.id.versionText)) setBuildDisplay(!showBuild);
		else if (v == findViewById(R.id.buttonOK)) this.dismiss();
	}
	
	private void setBuildDisplay (boolean showBuild) {
		Context ctx = getContext();
		TextView versionText = (TextView) findViewById(R.id.versionText);
		PackageInfo pi = null;
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
