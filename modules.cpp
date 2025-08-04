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