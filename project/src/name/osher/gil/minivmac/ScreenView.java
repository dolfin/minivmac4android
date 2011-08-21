package name.osher.gil.minivmac;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.*;

public class ScreenView extends View {
	private Bitmap screenBits;
	private int screenWidth, screenHeight;
	private Paint screenPaint;
	private Rect srcRect, dstRect;
	private boolean scaled;
	
	private void init() {
		screenWidth = Core.screenWidth();
		screenHeight = Core.screenHeight();
		screenBits = Bitmap.createBitmap(screenWidth, screenHeight, Bitmap.Config.RGB_565);
		screenPaint = new Paint();
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
	
	protected void onMeasure (int widthMeasureSpec, int heightMeasureSpec) {
		setMeasuredDimension(dstRect.width(), dstRect.height());
	}
	
	public void updateScreen(int[] update) {
		if (update.length < 4) return;
		int width = update[3]-update[1];
		int height = update[2]-update[0];
		screenBits.setPixels(update, 4, width, update[1], update[0], width, height);
		this.invalidate(); // FIXME invalidate only changed area
	}
	
	protected void onDraw (Canvas canvas) {
		canvas.drawBitmap(screenBits, srcRect, dstRect, screenPaint);
	}
	
	public boolean onTouchEvent (MotionEvent event) {
		int[] macCoords;
		macCoords = translateMouseCoords((int)event.getX(), (int)event.getY());
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			Core.setMousePos(macCoords[0], macCoords[1]);
			Core.setMouseButton(true);
			break;
		case MotionEvent.ACTION_MOVE:
			Core.setMousePos(macCoords[0], macCoords[1]);
			break;
		case MotionEvent.ACTION_CANCEL:
			Core.setMouseButton(false);
			break;
		case MotionEvent.ACTION_UP:
			Core.setMousePos(macCoords[0], macCoords[1]);
			Core.setMouseButton(false);
			break;
		}
		return true;
	}

	private int[] translateMouseCoords(int x, int y) {
		int[] coords = new int[2];
		if (scaled) {
			coords[0] = (x * srcRect.right) / dstRect.right;
			coords[1] = (y * srcRect.bottom) / dstRect.bottom;
		} else {
			coords[0] = x + srcRect.left;
			coords[1] = y + srcRect.top;
		}
		return coords;
	}

	public void setScaled(boolean scaled) {
		DisplayMetrics dm = new DisplayMetrics();
		WindowManager wm = (WindowManager)this.getContext().getSystemService(Context.WINDOW_SERVICE);
		wm.getDefaultDisplay().getMetrics(dm);
		
		int realHeight = dm.heightPixels;
		int realWidth = dm.widthPixels;
		
		if (dm.heightPixels > dm.widthPixels) {
			realHeight = dm.widthPixels;
			realWidth = dm.heightPixels;
		}
		
		Boolean perfectScale = false;
		if (((screenWidth * 2) <= realWidth) && ((screenHeight * 2) <= realHeight)) {
			perfectScale = true;
		} else if (scaled) {
			double aspectRatio = (double)screenWidth / screenHeight;
			realWidth = (int) (realHeight * aspectRatio);
		}
		
		this.scaled = scaled;
		screenPaint.setFilterBitmap(scaled);
		if (scaled) {
			if (perfectScale) {
				srcRect = new Rect(0, 0, realWidth/2, realHeight/2);
			} else {
				srcRect = new Rect(0, 0, screenWidth, screenHeight);
			}
			dstRect = new Rect(0, 0, realWidth, realHeight);
		} else {
			srcRect = new Rect(0, 0, realWidth, realHeight);
			dstRect = new Rect(0, 0, realWidth, realHeight);
		}
		invalidate();
	}
	
	public boolean isScaled() {
		return scaled;
	}
	
	public void scrollScreen(int keyCode, int increment) {
		int top,left;
		if (scaled) return;
		top = srcRect.top;
		left = srcRect.left;
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
		if (top < 0) top = 0;
		if (left < 0) left = 0;
		if (top + srcRect.height() > screenHeight) top = screenHeight - srcRect.height();
		if (left + srcRect.width() > screenWidth) left = screenWidth - srcRect.width();
		srcRect.offsetTo(left,top);
		invalidate();
	}
}
