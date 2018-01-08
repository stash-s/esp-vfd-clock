#ifndef __UTILS_HPP__
#define __UTILS_HPP__

/**
 * Utility class. Rotate through available numbers and trigger actions when ticking and reseting
 */
template <typename tick_callback_type, typename reset_callback_type>
class rotary_counter
{
    const int max_counter;
    int counter;
    const tick_callback_type tick_callback;
    const reset_callback_type reset_callback;

public:

    rotary_counter(int _dupa, tick_callback_type tick_callback, reset_callback_type reset_callback)
    :max_counter (_dupa),
     counter (_dupa-1),
     reset_callback (reset_callback),
     tick_callback (tick_callback)
        {}

    inline int next ()
        {
            ++ counter;
            if (counter == max_counter) {
                reset_callback (counter);
                counter = 0;
                tick_callback (counter);
            } else {
                tick_callback (counter);
            }

            return counter;
        }

    inline int value () const {
        return counter;
    }
};

template <typename t, typename q>
inline rotary_counter<t,q> make_rotary_counter (int max, t tick, q reset)
{
    return rotary_counter<t,q>(max, tick, reset);
}


#endif // __UTILS_HPP__
