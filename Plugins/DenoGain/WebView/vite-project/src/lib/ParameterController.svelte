<script lang="ts" context="module">
// Utility functions.
function mapRange(value, fromMin, fromMax, toMin, toMax) {
    const clampedValue = Math.min(Math.max(value, fromMin), fromMax);
    const normalizedValue = (clampedValue - fromMin) / (fromMax - fromMin);
    const mappedValue = normalizedValue * (toMax - toMin) + toMin;
    return mappedValue;
}

export function setupParameterController() {
    console.log('setupParameterController');
    // JavaScript code for handling button and slider events
    const gainSlider = document.getElementById("gainSlider") as HTMLInputElement;
    const gainValueLabel = document.getElementById("gainValueLabel") as HTMLSpanElement;

    const toggleInvertPhase = document.getElementById("toggleInvertPhase") as HTMLInputElement;
    const invertPhaseValueLabel = document.getElementById("invertPhaseValueLabel") as HTMLSpanElement;

    // Event listener for gain slider
    gainSlider.addEventListener('input', function() {
        const jsonData = {
            sliderName: gainSlider.id,
            sliderValue: gainSlider.value,
            sliderRangeMin: gainSlider.min,
            sliderRangeMax: gainSlider.max
        };
        
        gainValueLabel.innerText = gainSlider.value;

        (window as any).onSliderChanged(jsonData)
    });

    // Event listener for toggle invert phase
    toggleInvertPhase.addEventListener('change', function() {
        const jsonData = {
            toggleName: toggleInvertPhase.id,
            toggleValue: toggleInvertPhase.checked,
        };
        
        if(toggleInvertPhase.checked){
            invertPhaseValueLabel.innerText = "ON";
        }
        else{
            invertPhaseValueLabel.innerText = "OFF";
        }

        (window as any).onToggleChanged(jsonData)
    });
}

// Event callback from JUCE.
export function onParameterChanged(jsonData) {
    // This is the function that will be called from C++
    console.log('JSON from C++: onParameterChanged - ', jsonData);

    // Access the 'type' property in the JSON data
    const parameterName = jsonData.parameterName;

    // Update the HTML label with the message
    if(parameterName === "gain")
    {
        const gainSlider = document.getElementById("gainSlider") as HTMLInputElement;
        const gainValueLabel = document.getElementById("gainValueLabel") as HTMLSpanElement;

        const mapped_value = mapRange(jsonData.parameterValue, 0.0, 1.0, gainSlider.min, gainSlider.max)
        console.log('gainSlider: ', mapped_value);
        
        gainSlider.value = mapped_value;
        gainValueLabel.innerText = gainSlider.value;
    }
    else if(parameterName === "invertPhase")
    {
        const toggleInvertPhase = document.getElementById("toggleInvertPhase") as HTMLInputElement;
        const invertPhaseValueLabel = document.getElementById("invertPhaseValueLabel") as HTMLSpanElement;
        
        console.log('invertPhase: ', jsonData.parameterValue);

        toggleInvertPhase.checked = jsonData.parameterValue;
        if(toggleInvertPhase.checked){
            invertPhaseValueLabel.innerHTML = "ON";
        }
        else{
            invertPhaseValueLabel.innerText = "OFF";
        }
    }
}
</script>

<div>
    <p>Gain [<span id="gainValueLabel">50</span>]</p>
    <input type="range" min="0" max="100" value="50" id="gainSlider">

    <p>Invert Phase [<span id="invertPhaseValueLabel"></span>]</p>
    <input type="checkbox" id="toggleInvertPhase">
</div>
