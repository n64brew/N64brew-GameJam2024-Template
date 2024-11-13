#ifndef CONTROLS_H
#define CONTROLS_H


typedef struct {

    joypad_buttons_t pressed;
    joypad_buttons_t held;
    joypad_buttons_t released;
    joypad_inputs_t input;

    // RUMBLE
    bool rumble_active;
    uint8_t rumble_time;

} ControllerData;


void controllerData_getInputs(ControllerData*data, uint8_t port);
void controllerData_rumbleStart(ControllerData* data, uint8_t port);
void controllerData_rumbleStop(ControllerData* data, uint8_t port);
void controllerData_rumbleFrames(ControllerData*data, uint8_t port, uint8_t frames);

void controllerData_getInputs(ControllerData* data, uint8_t port)
{
    data->pressed = joypad_get_buttons_pressed(port);
    data->held = joypad_get_buttons_held(port);
    data->released = joypad_get_buttons_released(port);
    data->input = joypad_get_inputs(port);

    // @TODO: Not sure if this is expensive to do every tick, but it may allow hotloading a pak
    if (!joypad_get_rumble_supported(port)) controllerData_rumbleStop(data, port);

}

/* RUMBLE */

// Set rumble to active state
void controllerData_rumbleStart(ControllerData* data, uint8_t port)
{
    joypad_set_rumble_active(port, true);
    data->rumble_time = 0;
    data->rumble_active = true;
}

// Reset rumble to idle state
 void controllerData_rumbleStop(ControllerData* data, uint8_t port)
{
    joypad_set_rumble_active(port, false);
    data->rumble_time = 0;
    data->rumble_active = false;
}

// Rumble controller for n number of frames
void controllerData_rumbleFrames(ControllerData*data, uint8_t port, uint8_t frames)
{

    if (data->rumble_active == false)
    {
        controllerData_rumbleStart(data, port);
    }

    data->rumble_time++;

    if (data->rumble_time >= frames) controllerData_rumbleStop(data, port);

}


#endif