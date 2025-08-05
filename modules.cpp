#include "systemc.h"
#include "digit.h"
#include "modules.h"

void splitter::run_splitter()
{
	out_data_h.write(HIGH_HALF(in_data.read()));
	out_data_l.write(LOW_HALF(in_data.read()));
}

void multiplier::run_multiplier()
{
	out_data.write(in_data_1.read() * in_data_2.read());
}

void multiplexor::run_multiplexor()
{
	if (select.read() == 1)
	{
		out_data.write(in_data_1.read());
	}
	else
	{
		out_data.write(in_data_2.read());
	}
}

void comparator::run_comparator()
{
	if (in_data_2.read() > in_data_1.read())
	{
		less_than_signal.write(1);
	}
	else
	{
		less_than_signal.write(0);
	}
}

void subtractor::run_subtractor()
{
	out_data.write(in_data_1.read() - in_data_2.read());
}

void bit_shift::run_bit_shift()
{
    out_data.write(in_data.read() << 16);
}

void reg::run_reg()
{
	while(true)
	{
		if (load_data.read() == 1)
		{	
			out_data.write(in_data.read());
		}
		
		wait();
	}
}

void half_reg::run_reg()
{
	while(true)
	{
		if (load_data.read() == 1)
		{	
			out_data.write(in_data.read());
		}
		
		wait();
	}
}


void zero_extender::run_extender()
{
    out_data.write((NN_DIGIT)in_data.read()); 
}

void loop_engine::run() {
    t0_out.write(0);
    t1_out.write(0);
    aHigh_out.write(0);
    done.write(false);

    wait();

    while (true) {
        done.write(false);

        if (start.read()) {
            NN_DIGIT t0 = t0_in.read();
            NN_DIGIT t1 = t1_in.read();
            NN_HALF_DIGIT aHigh = aHigh_in.read();

            // Truncate internally from full-width inputs
            NN_HALF_DIGIT cH = (NN_HALF_DIGIT)cHigh.read();
            NN_HALF_DIGIT cL = (NN_HALF_DIGIT)cLow.read();
            NN_DIGIT cL_hi = TO_HIGH_HALF(cL);

            while ((t1 > cH) || ((t1 == cH) && (t0 >= cL_hi))) {
                if ((t0 -= cL_hi) > (MAX_NN_DIGIT - cL_hi)) t1--;
                t1 -= cH;
                aHigh++;
            }

            t0_out.write(t0);
            t1_out.write(t1);
            aHigh_out.write(aHigh);  
            done.write(true);
        }

        wait();
    }
}

enum State {
    WAIT,
    LOAD_INPUT,
    WAIT_STABILIZE,
    RESET_LOOP,
    START_LOOP,
    WAIT_LOOP_DONE,
    WRITE_OUTPUT,
    FINISH
};

void controller::fsm() {
    State state = WAIT;

    // Default all outputs
    load_input.write(false);
    load_output.write(false);
    loop_start.write(false);
    loop_reset.write(false);
    hw_done.write(false);

    wait();

    while (true) {
        switch (state) {
            case WAIT:
                hw_done.write(false);
                if (hw_enable.read())
                    state = LOAD_INPUT;
                break;

            case LOAD_INPUT:
                load_input.write(true);
                state = WAIT_STABILIZE;
                break;

            case WAIT_STABILIZE:
                load_input.write(false);
                state = RESET_LOOP;
                break;

            case RESET_LOOP:
                loop_reset.write(true);
                state = START_LOOP;
                break;

            case START_LOOP:
                loop_reset.write(false);
                loop_start.write(true);
                state = WAIT_LOOP_DONE;
                break;

            case WAIT_LOOP_DONE:
                loop_start.write(false);
                if (loop_done.read())
                    state = WRITE_OUTPUT;
                break;

            case WRITE_OUTPUT:
                load_output.write(true);
                state = FINISH;
                break;

            case FINISH:
                load_output.write(false);
                hw_done.write(true);
                if (!hw_enable.read()) {
                    hw_done.write(false);
                    state = WAIT;
                }
                break;
        }

        wait();
    }
}