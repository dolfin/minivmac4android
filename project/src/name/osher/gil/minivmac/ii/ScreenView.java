package name.osher.gil.minivmac.ii;

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
	private int targetScreenWidth, targetScreenHeight;
	private Paint screenPaint;
	private Rect srcRect, dstRect;
	private boolean scaled, scroll;
	
	private void init() {
		targetScreenWidth = Core.screenWidth();
		targetScreenHeight = Core.screenHeight();
		screenBits = Bitmap.createBitmap(targetScreenWidth, targetScreenHeight, Bitmap.Config.RGB_565);
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
		setMeasuredDimension(widthMeasureSpec, heightMeasureSpec);
		setScaled(isScaled());
	}
	
	public void updateScreen(int[] update) {
		if (update.length < 4) return;
		int width = update[3]-update[1];
		int height = update[2]-update[0];
		screenBits.setPixels(update, 4, width, update[1], update[0], width, height);
		this.invalidate(); // FIXME invalidate only changed area
	}
	
	protected void onDraw (Canvas canvas) {
		canvas.drawBitmap(screenBits, null, dstRect, screenPaint);
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
		coords[0] = (int)((x - dstRect.left) * (srcRect.right / (double)dstRect.width()));
		coords[1] = (int)((y - dstRect.top) * (srcRect.bottom / (double)dstRect.height()));
		return coords;
	}

	public void setScaled(boolean scaled) {
		this.scaled = scaled;
		screenPaint.setFilterBitmap(scaled);
		
		DisplayMetrics dm = new DisplayMetrics();
		WindowManager wm = (WindowManager)this.getContext().getSystemService(Context.WINDOW_SERVICE);
		wm.getDefaultDisplay().getMetrics(dm);
		
		int hostScreenWidth = dm.widthPixels;
		int hostScreenHeight = dm.heightPixels;
		
		
		double perfectWidthFactor = Math.floor((double)hostScreenWidth / (double)targetScreenWidth);
		double perfectHeightFactor = Math.floor((double)hostScreenHeight / (double)targetScreenHeight);
		double scaleFactor = Math.min(perfectWidthFactor, perfectHeightFactor);
		if (scaleFactor < 1.0) scaleFactor = 1.0;
	
		if (scaled) {
			scaleFactor = Math.min( (double)hostScreenWidth/(double)targetScreenWidth, (double)hostScreenHeight/(double)targetScreenHeight);
		}
		
		int surfaceHeight = (int)(targetScreenHeight * scaleFactor);
		int surfaceWidth = (int)(targetScreenWidth * scaleFactor);
		
		int left = (hostScreenWidth - surfaceWidth)/2;
		int top = (hostScreenHeight - surfaceHeight)/2;
		if (left < 0) left = 0;
		if (top < 0) top = 0;
		dstRect = new Rect(left, top, left + surfaceWidth, top + surfaceHeight);
		srcRect = new Rect(0, 0, targetScreenWidth, targetScreenHeight);
		
		invalidate();
	}
	
	public boolean isScaled() {
		return scaled;
	}
	
	public void setScroll(boolean scroll) {
		this.scroll = scroll;
	}
	
	public boolean isScroll() {
		return scroll;
	}
	
	public void scrollScreen(int keyCode, int increment) {
		int top,left;
		if (!scroll) return;
		if (scaled) return;
		top = dstRect.top;
		left = dstRect.left;
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
		
		if (hostScreenHeight < targetScreenHeight) {
			if (top > 0) top = 0;
			if (top < (hostScreenHeight - dstRect.height()))
				top = hostScreenHeight - dstRect.height();
		}
		else
		{
			if (top < 0) top = 0;
			if (top + dstRect.height() > hostScreenHeight) top = hostScreenHeight - dstRect.height();
		}
		
		if (hostScreenWidth < targetScreenWidth) {
			if (left >0) left = 0;
			if (left < (hostScreenWidth - dstRect.width())) 
				left = hostScreenWidth - dstRect.width();
		}
		else
		{
			if (left < 0) left = 0;
			if (left + dstRect.width() > hostScreenWidth) left = hostScreenWidth - dstRect.width();
		}
		
		dstRect.offsetTo(left,top);
		invalidate();
	}
}
