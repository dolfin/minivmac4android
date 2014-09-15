package name.osher.gil.minivmac;

import name.osher.gil.minivmac.R;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;

public class Key extends LinearLayout implements View.OnTouchListener {

	private String mText;
	private int mScanCode;
	private int mDownImage;
	private int mUpImage;
	private int mHoldImage;
	private boolean mIsStickyDown = false;
	
	private Button mButton;
	
	public Key(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		TypedArray a = getContext().obtainStyledAttributes(attrs,R.styleable.Key);
		mText = a.getString(R.styleable.Key_text);
		mScanCode = a.getInteger(R.styleable.Key_scanCode, -1);
		mDownImage = a.getResourceId(R.styleable.Key_downImage, R.drawable.kb_key_down);
		mUpImage = a.getResourceId(R.styleable.Key_upImage, R.drawable.kb_key_up);
		mHoldImage = a.getResourceId(R.styleable.Key_holdImage, -1);
		a.recycle();
		
		mButton = new Button(context, attrs);
		mButton.setText(mText);
		mButton.setTextColor(Color.BLACK);
		mButton.setBackgroundResource(mUpImage);
		mButton.setOnTouchListener(this);
		this.addView(mButton);
	}

	public boolean onTouch(View v, MotionEvent event) {
		if (mScanCode == -1) return false;
		
		if (event.getAction() == MotionEvent.ACTION_DOWN)
		{
			mButton.setBackgroundResource(mDownImage);

			if(isStickyKey()) {
				if (!mIsStickyDown) {
					Core.setKeyDown(mScanCode);
					mIsStickyDown = true;
				} else {
					mIsStickyDown = false;
				}
			} else {
				Core.setKeyDown(mScanCode);
			}
			
			return true;
		} else if (event.getAction() == MotionEvent.ACTION_UP) {
			if(mIsStickyDown) {
				mButton.setBackgroundResource(mHoldImage);
			} else {
				mButton.setBackgroundResource(mUpImage);
				Core.setKeyUp(mScanCode);
			}
			return true;
		}
		
		return false;
	}
	
	protected boolean isStickyKey() {
		return (mScanCode == 55 || mScanCode == 56 || mScanCode == 58 || mScanCode == 59); // Command, Options, Shift, Control
	}

}
