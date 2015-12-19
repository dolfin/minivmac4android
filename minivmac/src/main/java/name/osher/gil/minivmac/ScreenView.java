package name.osher.gil.minivmac;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.*;

public class ScreenView extends View {
	private Bitmap mScreenBits = null;
	private int mTargetScreenWidth = 0, mTargetScreenHeight = 0;
	private Paint mScreenPaint;
	private Rect mSrcRect, mDstRect;
	private boolean mScaled, mScroll;
	private OnMouseEventListener mListener;
	
	private void init() {
		if (isInEditMode()) {
			mTargetScreenWidth = 512;
			mTargetScreenHeight = 320;
		}

		mScreenPaint = new Paint();
		setScaled(false);
	}
	
	public ScreenView(Context context) {
		super(context);
		init();
	}

	public ScreenView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public ScreenView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public void setTargetScreenSize(int width, int height) {
		mTargetScreenWidth = width;
		mTargetScreenHeight = height;
		mScreenBits = Bitmap.createBitmap(mTargetScreenWidth, mTargetScreenHeight, Bitmap.Config.RGB_565);
	}

	public void setOnMouseEventListener(OnMouseEventListener listener) {
		mListener = listener;
	}
	
	protected void onMeasure (int widthMeasureSpec, int heightMeasureSpec) {
		setMeasuredDimension(widthMeasureSpec, heightMeasureSpec);
		setScaled(isScaled());
	}
	
	public void updateScreen(int[] update) {
		if (mScreenBits == null || update.length < 4) return;
		int width = update[3]-update[1];
		int height = update[2]-update[0];
		mScreenBits.setPixels(update, 4, width, update[1], update[0], width, height);
		this.invalidate(); // FIXME invalidate only changed area
	}
	
	protected void onDraw (Canvas canvas) {
		if (mScreenBits != null) {
			canvas.drawBitmap(mScreenBits, null, mDstRect, mScreenPaint);
		}
	}
	
	public boolean onTouchEvent (@NonNull MotionEvent event) {
		int[] macCoords;
		macCoords = translateMouseCoords((int)event.getX(), (int)event.getY());
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			mListener.onMouseMove(macCoords[0], macCoords[1]);
			mListener.onMouseClick(true);
			return true;
		case MotionEvent.ACTION_MOVE:
			mListener.onMouseMove(macCoords[0], macCoords[1]);
            return true;
		case MotionEvent.ACTION_CANCEL:
			mListener.onMouseClick(false);
            return true;
		case MotionEvent.ACTION_UP:
			mListener.onMouseMove(macCoords[0], macCoords[1]);
			mListener.onMouseClick(false);
            return true;
		}
		return super.onTouchEvent(event);
	}

    @TargetApi(12)
    public boolean onGenericMotionEvent (MotionEvent event) {
        if (event.getSource() == InputDevice.SOURCE_MOUSE) {
            int[] macCoords;
            macCoords = translateMouseCoords((int)event.getX(), (int)event.getY());

            switch (event.getAction()) {
                case MotionEvent.ACTION_HOVER_MOVE:
                    mListener.onMouseMove(macCoords[0], macCoords[1]);
                    return true;
            }
        }

        return super.onGenericMotionEvent(event);
    }

	private int[] translateMouseCoords(int x, int y) {
		int[] coords = new int[2];
		coords[0] = (int)((x - mDstRect.left) * (mSrcRect.right / (double) mDstRect.width()));
		coords[1] = (int)((y - mDstRect.top) * (mSrcRect.bottom / (double) mDstRect.height()));
		return coords;
	}

	public void setScaled(boolean scaled) {
		this.mScaled = scaled;
		mScreenPaint.setFilterBitmap(scaled);

		int hostScreenWidth = getMeasuredWidth();
		int hostScreenHeight = getMeasuredHeight();
		
		double perfectWidthFactor = Math.floor((double)hostScreenWidth / (double) mTargetScreenWidth);
		double perfectHeightFactor = Math.floor((double)hostScreenHeight / (double) mTargetScreenHeight);
		double scaleFactor = Math.min(perfectWidthFactor, perfectHeightFactor);
		if (scaleFactor < 1.0) scaleFactor = 1.0;
	
		if (scaled) {
			scaleFactor = Math.min( (double)hostScreenWidth/(double) mTargetScreenWidth, (double)hostScreenHeight/(double) mTargetScreenHeight);
		}
		
		int surfaceHeight = (int)(mTargetScreenHeight * scaleFactor);
		int surfaceWidth = (int)(mTargetScreenWidth * scaleFactor);
		
		int left = (hostScreenWidth - surfaceWidth)/2;
		int top = (hostScreenHeight - surfaceHeight)/2;
		if (left < 0) left = 0;
		if (top < 0) top = 0;
		mDstRect = new Rect(left, top, left + surfaceWidth, top + surfaceHeight);
		mSrcRect = new Rect(0, 0, mTargetScreenWidth, mTargetScreenHeight);
		
		invalidate();
	}
	
	public boolean isScaled() {
		return mScaled;
	}
	
	public void setScroll(boolean scroll) {
		this.mScroll = scroll;
	}
	
	public boolean isScroll() {
		return mScroll;
	}
	
	public void scrollScreen(int keyCode, int increment) {
		int top,left;
		if (!mScroll) return;
		if (mScaled) return;
		top = mDstRect.top;
		left = mDstRect.left;
		switch(keyCode) {
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			left += increment;
			break;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			left -= increment;
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			top -= increment;
			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			top += increment;
			break;
		}
		
		DisplayMetrics dm = new DisplayMetrics();
		WindowManager wm = (WindowManager)this.getContext().getSystemService(Context.WINDOW_SERVICE);
		wm.getDefaultDisplay().getMetrics(dm);
		
		int hostScreenWidth = dm.widthPixels;
		int hostScreenHeight = dm.heightPixels;
		
		if (hostScreenHeight < mTargetScreenHeight) {
			if (top > 0) top = 0;
			if (top < (hostScreenHeight - mDstRect.height()))
				top = hostScreenHeight - mDstRect.height();
		}
		else
		{
			if (top < 0) top = 0;
			if (top + mDstRect.height() > hostScreenHeight) top = hostScreenHeight - mDstRect.height();
		}
		
		if (hostScreenWidth < mTargetScreenWidth) {
			if (left >0) left = 0;
			if (left < (hostScreenWidth - mDstRect.width()))
				left = hostScreenWidth - mDstRect.width();
		}
		else
		{
			if (left < 0) left = 0;
			if (left + mDstRect.width() > hostScreenWidth) left = hostScreenWidth - mDstRect.width();
		}
		
		mDstRect.offsetTo(left, top);
		invalidate();
	}

	public interface OnMouseEventListener {
		void onMouseMove(int x, int y);
		void onMouseClick(boolean down);
	}
}
