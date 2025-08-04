#include "systemc.h"
#include "digit.h"
#include "dh_hw.h"

enum HWState { WAIT, EXECUTE, FINISH };

void dh_hw::process_hw() {
  HWState state = WAIT;
  hw_done.write(false);
  

  // Internal variables
  NN_DIGIT t[2], u_val, v_val, t1_after_dec;
  NN_HALF_DIGIT aHigh_val, cHigh, cLow;

  wait(); // Initial clock sync

  while (true) {
    switch (state) {
      case WAIT:
        hw_done.write(false);
        if (hw_enable.read()) {
          state = EXECUTE;
        }
        break;

      case EXECUTE:
        // Read inputs (via signals now)
        t[0] = from_sw0.read();
        t[1] = from_sw1.read();
        c.write(from_sw2.read());
        aHigh_val = from_sw3.read();
        aHigh.write(aHigh_val);  // drive signal for datapath

        wait();
        
        u_val = u.read();
        v_val = v.read();
  
        cHigh = (NN_HALF_DIGIT)c_high.read();
        cLow  = (NN_HALF_DIGIT)c_low.read();

        t[0] = t0_sub.read();
        t1_after_dec = t1_output.read();

        while ((t1_after_dec > cHigh) || ((t1_after_dec == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
            if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow)) t1_after_dec--;
            t1_after_dec -= cHigh;
            aHigh_val++;
        }
        

        // Write outputs
        to_sw0.write(t[0]);
        to_sw1.write(t1_after_dec);
        to_sw2.write(aHigh_val);

        state = FINISH;
        break;

      case FINISH:
        hw_done.write(true);
        if (!hw_enable.read()) {
          hw_done.write(false);
          state = WAIT;
        }
        break;
    }
    
    wait();
    
    wait();
    wait();
  }
}

