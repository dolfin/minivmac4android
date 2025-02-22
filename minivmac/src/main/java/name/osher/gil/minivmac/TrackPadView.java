package name.osher.gil.minivmac;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.View;

import java.util.HashSet;
import java.util.Set;

public class TrackPadView extends View {

    // thresholds and acceleration constants
    private static final long TOUCH_TIME_THRESHOLD_MS = 250; // 0.25 seconds
    private static final float TOUCH_DISTANCE_THRESHOLD = 16; // in pixels
    private static final float TRACKPAD_ACCEL_N = 0.2f;
    private static final float TRACKPAD_ACCEL_T = 0.2f;
    private static final float TRACKPAD_ACCEL_D = 20;

    // state variables
    private long previousTouchTime = 0;
    private float previousTouchX = 0;
    private float previousTouchY = 0;
    private boolean shouldClick = false;
    private boolean isDragging = false;
    private boolean didForceClick = false;
    // In Android, force touch isn’t widely supported so we leave this false.
    private boolean supportsForceTouch = false;

    // Track active pointer IDs
    private final Set<Integer> currentTouches = new HashSet<>();
    private int activePointerId = -1;

    // Runnables for scheduled click and mouse up
    private Runnable mouseClickRunnable;
    private Runnable mouseUpRunnable;

    private ScreenView.OnMouseEventListener mListener;

    // Handler (using the view’s post methods)
    private final Handler handler = new Handler();

    public TrackPadView(Context context) {
        super(context);
        init();
    }

    public TrackPadView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public TrackPadView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        // Any additional initialization if needed.
        // (For example, you might decide whether to support force touch based on device capabilities.)
    }

    public void setOnMouseEventListener(ScreenView.OnMouseEventListener listener) {
        mListener = listener;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        long eventTime = event.getEventTime(); // in ms

        // Check for mouse events (if a hardware mouse is used)
        if (event.getButtonState() != 0) {
            if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_BUTTON_PRESS) {
                startDragging();
            } else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_BUTTON_RELEASE) {
                stopDragging();
            }
            return true;
        }

        switch (action) {
            case MotionEvent.ACTION_DOWN: {
                // First finger touches down.
                activePointerId = event.getPointerId(0);
                currentTouches.add(activePointerId);
                firstTouchBegan(event, 0);
                break;
            }
            case MotionEvent.ACTION_POINTER_DOWN: {
                // An additional finger touches down.
                int index = event.getActionIndex();
                int pointerId = event.getPointerId(index);
                currentTouches.add(pointerId);
                if (currentTouches.size() > 1) {
                    startDragging();
                }
                break;
            }
            case MotionEvent.ACTION_MOVE: {
                int pointerIndex = event.findPointerIndex(activePointerId);
                if (pointerIndex == -1) break;

                // Process all historical points for smoother movement.
                int historySize = event.getHistorySize();
                for (int i = 0; i < historySize; i++) {
                    float historicalX = event.getHistoricalX(pointerIndex, i);
                    float historicalY = event.getHistoricalY(pointerIndex, i);

                    // Calculate differences from previous point
                    float diffX = historicalX - previousTouchX;
                    float diffY = historicalY - previousTouchY;

                    // Compute elapsed time (convert from ms to sec)
                    float dt = (event.getHistoricalEventTime(i) - previousTouchTime) / 1000f;
                    float timeDiff = 100 * dt;
                    float accel = TRACKPAD_ACCEL_N / (TRACKPAD_ACCEL_T + ((timeDiff * timeDiff) / TRACKPAD_ACCEL_D));
                    diffX *= accel;
                    diffY *= accel;

                    if (diffX != 0 || diffY != 0) {
                        shouldClick = false;
                        mListener.onMouseMove((int)diffX, (int)diffY);
                    }

                    // Update previous point
                    previousTouchX = historicalX;
                    previousTouchY = historicalY;
                    previousTouchTime = event.getHistoricalEventTime(i);
                }

                // Process current point as well
                float currentX = event.getX(pointerIndex);
                float currentY = event.getY(pointerIndex);
                float diffX = currentX - previousTouchX;
                float diffY = currentY - previousTouchY;
                float dt = (event.getEventTime() - previousTouchTime) / 1000f;
                float timeDiff = 100 * dt;
                float accel = TRACKPAD_ACCEL_N / (TRACKPAD_ACCEL_T + ((timeDiff * timeDiff) / TRACKPAD_ACCEL_D));
                diffX *= accel;
                diffY *= accel;

                if (diffX != 0 || diffY != 0) {
                    shouldClick = false;
                    mListener.onMouseMove((int)diffX, (int)diffY);
                }

                previousTouchX = currentX;
                previousTouchY = currentY;
                previousTouchTime = event.getEventTime();

                // Optionally handle force touch if supported.
                if (supportsForceTouch) {
                    float pressure = event.getPressure(pointerIndex);
                    if (pressure > 0.8f && !didForceClick) {
                        performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
                        didForceClick = true;
                        startDragging();
                    }
                }
                break;
            }
            case MotionEvent.ACTION_UP: {
                // Last finger lifted.
                currentTouches.remove(activePointerId);
                if (!currentTouches.isEmpty()) {
                    // Other touches still remain.
                    break;
                } else if (didForceClick) {
                    // If a force click occurred, provide feedback and end dragging.
                    performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
                    didForceClick = false;
                    cancelScheduledClick();
                    mouseUp();
                    return true;
                }
                // If it was a tap (short time) then schedule a click.
                if (shouldClick && (eventTime - previousTouchTime < TOUCH_TIME_THRESHOLD_MS)) {
                    cancelScheduledClick();
                    postDelayed(getMouseClickRunnable(), TOUCH_TIME_THRESHOLD_MS);
                }
                shouldClick = false;
                if (isDragging) {
                    stopDragging();
                }
                previousTouchX = event.getX();
                previousTouchY = event.getY();
                previousTouchTime = eventTime;
                activePointerId = -1;
                break;
            }
            case MotionEvent.ACTION_POINTER_UP: {
                // A non-primary finger was lifted.
                if (isDragging) {
                    stopDragging();
                }

                int pointerIndex = event.getActionIndex();
                int pointerId = event.getPointerId(pointerIndex);
                currentTouches.remove(pointerId);
                if (currentTouches.isEmpty()) {
                    if (didForceClick) {
                        performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
                        didForceClick = false;
                        cancelScheduledClick();
                        mouseUp();
                        return true;
                    }
                    if (shouldClick && (eventTime - previousTouchTime < TOUCH_TIME_THRESHOLD_MS)) {
                        cancelScheduledClick();
                        postDelayed(getMouseClickRunnable(), TOUCH_TIME_THRESHOLD_MS);
                    }
                    shouldClick = false;
                    if (isDragging) {
                        stopDragging();
                    }
                    // Use the coordinates of the pointer that just lifted.
                    previousTouchX = event.getX(pointerIndex);
                    previousTouchY = event.getY(pointerIndex);
                    previousTouchTime = eventTime;
                    activePointerId = -1;
                }
                break;
            }
            case MotionEvent.ACTION_CANCEL: {
                currentTouches.clear();
                isDragging = false;
                shouldClick = false;
                didForceClick = false;
                mouseUp();
                break;
            }
        }
        return true;
    }

    /**
     * Called when the first touch is detected.
     */
    private void firstTouchBegan(MotionEvent event, int pointerIndex) {
        float touchX = event.getX(pointerIndex);
        float touchY = event.getY(pointerIndex);
        shouldClick = true;
        if (!isDragging &&
                (event.getEventTime() - previousTouchTime < TOUCH_TIME_THRESHOLD_MS) &&
                Math.abs(previousTouchX - touchX) < TOUCH_DISTANCE_THRESHOLD &&
                Math.abs(previousTouchY - touchY) < TOUCH_DISTANCE_THRESHOLD) {
            startDragging();
        }
        previousTouchTime = event.getEventTime();
        previousTouchX = touchX;
        previousTouchY = touchY;
    }

    /**
     * Begins a dragging (mouse down) state.
     */
    private void startDragging() {
        isDragging = true;
        shouldClick = false;
        mListener.onMouseClick(true);
    }

    /**
     * Ends the dragging (mouse up) state.
     */
    private void stopDragging() {
        isDragging = false;
        shouldClick = false;
        mListener.onMouseClick(false);
    }

    /**
     * Returns a Runnable that simulates a mouse click.
     */
    private Runnable getMouseClickRunnable() {
        if (mouseClickRunnable == null) {
            mouseClickRunnable = new Runnable() {
                @Override
                public void run() {
                    mouseClick();
                }
            };
        }
        return mouseClickRunnable;
    }

    /**
     * Returns a Runnable that simulates the mouse button being released.
     */
    private Runnable getMouseUpRunnable() {
        if (mouseUpRunnable == null) {
            mouseUpRunnable = new Runnable() {
                @Override
                public void run() {
                    mouseUp();
                }
            };
        }
        return mouseUpRunnable;
    }

    /**
     * Performs a click if not dragging.
     */
    private void mouseClick() {
        if (isDragging) return;
        mListener.onMouseClick(true);
        // Schedule mouse up after ~33ms (approx. 2/60 seconds)
        postDelayed(getMouseUpRunnable(), 33);
    }

    /**
     * Cancels any scheduled click or mouse-up actions.
     */
    private void cancelScheduledClick() {
        removeCallbacks(getMouseClickRunnable());
        removeCallbacks(getMouseUpRunnable());
    }

    /**
     * Sends a mouse-up command to the emulator.
     */
    private void mouseUp() {
        mListener.onMouseClick(false);
    }
}