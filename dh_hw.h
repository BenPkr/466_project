#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "modules.h"

SC_MODULE (dh_hw)
{
  sc_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_in <NN_HALF_DIGIT> from_sw3;

  sc_out <NN_DIGIT> to_sw0, to_sw1;
  sc_out <NN_HALF_DIGIT> to_sw2;

  sc_in<bool> clk;
  
  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done;

  // interconnect signals for multipliers and splitters
  sc_signal<NN_DIGIT> c, c_high, c_low;
  sc_signal<NN_DIGIT> aHigh, u, v;
  // interconnect signals for the multiplexer and comparator
  sc_signal <NN_DIGIT> u_high, t0_sub, u_sub, t1_decremented, t1_after_conditional_decrement;
  sc_signal <NN_DIGIT> max_val, one;
  sc_signal<bool> comparator_sig;
  // interconnect signals for the final subtraction
  sc_signal<NN_DIGIT> u_upper_half, stub, t1_minus_u_out, t1_output;


  //modules
  //first split and multiply
  splitter c_splitter;
  multiplier u_multiplier, v_multiplier;

  //conditional decrement
  subtractor t0_subtractor, u_subtractor, t1_subtractor;
  bit_shift u_shifter;
  comparator dec_comparator;
  multiplexor dec_multiplexor;

  //final subtraction
  subtractor t1_minus_u, t1_minus_v;
  splitter u_splitter;

  void process_hw();
  
  SC_CTOR (dh_hw):
  c_splitter("c_splitter"),
  u_multiplier("u_multiplier"), v_multiplier("v_multiplier"),
  t0_subtractor("t0_subtractor"), u_subtractor("u_subtractor"),
  t1_subtractor("t1_subtractor"), u_shifter("u_shifter"),
  dec_comparator("dec_comparator"), dec_multiplexor("dec_multiplexor"),
  t1_minus_u("t1_minus_u"), t1_minus_v("t1_minus_v"), u_splitter("u_splitter")
  {
    max_val.write(0xFFFFFFFF);
    one.write(1);
    //first splitter connection
    c_splitter.in_data(c);
    c_splitter.out_data_h(c_high);
    c_splitter.out_data_l(c_low);

    //first and second multiplier connections
    u_multiplier.in_data_1(aHigh);
    u_multiplier.in_data_2(c_low);
    u_multiplier.out_data(u);

    v_multiplier.in_data_1(aHigh);
    v_multiplier.in_data_2(c_high);
    v_multiplier.out_data(v);

    //u splitter
    u_shifter.in_data(u);
    u_shifter.out_data(u_high);

    //t[0] -= TO_HIGH_HALF (u)
    t0_subtractor.in_data_1(from_sw0);
    t0_subtractor.in_data_2(u_high); 
    t0_subtractor.out_data(t0_sub);

    //MAX_NN_DIGIT - TO_HIGH_HALF (u)
    u_subtractor.in_data_1(max_val);
    u_subtractor.in_data_2(u_high); 
    u_subtractor.out_data(u_sub);

    // ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))

    dec_comparator.in_data_1(t0_sub);
    dec_comparator.in_data_2(u_sub);  
    dec_comparator.less_than_signal(comparator_sig); // Use u_high as the signal to indicate if decrement is needed

    //t[1]--
    t1_subtractor.in_data_1(from_sw1);
    t1_subtractor.in_data_2(one); 
    t1_subtractor.out_data(t1_decremented);

    // if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
    dec_multiplexor.in_data_1(from_sw1);
    dec_multiplexor.in_data_2(t1_decremented);
    dec_multiplexor.select(comparator_sig);
    dec_multiplexor.out_data(t1_after_conditional_decrement);

    u_splitter.in_data(u);
    u_splitter.out_data_h(u_upper_half);
    u_splitter.out_data_l(stub);

    t1_minus_u.in_data_1(t1_after_conditional_decrement);
    t1_minus_u.in_data_2(u_upper_half);
    t1_minus_u.out_data(t1_minus_u_out);

    t1_minus_v.in_data_1(t1_minus_u_out);
    t1_minus_v.in_data_2(v);
    t1_minus_v.out_data(t1_output);

    SC_CTHREAD(process_hw, clk.pos());
  }
  
};

#endif /* end _DH_HW_H_ */
