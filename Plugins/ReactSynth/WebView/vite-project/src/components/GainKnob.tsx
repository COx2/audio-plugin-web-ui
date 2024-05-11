// @ts-ignore
import { React, useState } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
import useWindowFunction from '../native_bridges/useWindowFunction';

const GainKnob = () => {
    const [value] = useState(0)

    return (
        <Knob 
          size={100}  
          angleOffset={220} 
          angleRange={280}
          min={0}
          max={100}
          // @ts-ignore
          onChange={value => console.log(value)}
        >
          <Arc 
            arcWidth={5}
            color="#FC5A96"
            radius={47.5} 
          />
          <Pointer 
            width={5}
            radius={40}
            type="circle"
            color="000000"
          />
          <Value 
            marginBottom={40} 
            className="value" 
          />
        </Knob>
    );
};

export default GainKnob;