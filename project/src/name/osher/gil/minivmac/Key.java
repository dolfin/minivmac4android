package name.osher.gil.minivmac;

import name.osher.gil.minivmac.R;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;

public class Key extends Button implements View.OnTouchListener {

	private int mScanCode;
	private int mDownImage;
	private int mUpImage;
	private int mHoldImage;
	private boolean mIsStickyDown = false;
	
	public Key(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		TypedArray a = getContext().obtainStyledAttributes(attrs,R.styleable.Key);
		mScanCode = a.getInteger(R.styleable.Key_scanCode, -1);
		mDownImage = a.getResourceId(R.styleable.Key_downImage, R.drawable.kb_key_down);
		mUpImage = a.getResourceId(R.styleable.Key_upImage, R.drawable.kb_key_up);
		mHoldImage = a.getResourceId(R.styleable.Key_holdImage, -1);
		
		this.setTextColor(Color.BLACK);
		
		this.setBackgroundResource(mUpImage);
		
		this.setOnTouchListener(this);
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		if (mScanCode == -1) return false;
		
		if (event.getAction() == MotionEvent.ACTION_DOWN)
		{
			this.setBackgroundResource(mDownImage);

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
				this.setBackgroundResource(mHoldImage);
			} else {
				this.setBackgroundResource(mUpImage);
				Core.setKeyUp(mScanCode);
			}
			return true;
		}
		
		return false;
	}
	
	protected boolean isStickyKey() {
		return (mScanCode == 55 || mScanCode == 56 || mScanCode == 58); // Command, Options, Shift
	}

}
