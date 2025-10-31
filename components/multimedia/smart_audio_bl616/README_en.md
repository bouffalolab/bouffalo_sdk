## Overview

In intelligent voice control systems, voice playback is a complex scenario involving a mix of various operations. The specific manifestations are as follows:

* Sound sources: They may be local, from the cloud, or other playback sources such as Bluetooth music and Bluetooth calls.
* Sound types: Including local prompt tones, TTS (Text-To-Speech) delivered by the cloud, music links delivered by the cloud, 
Bluetooth music playback, and Bluetooth call types; each type of sound is handled slightly differently.
* Diverse behaviors: Users may initiate a variety of playback control requests, such as volume control, pause/resume playback, and mute control.
* Arbitration management: Determine the playback priority when multiple playback requests arise simultaneously.

Therefore, a player control component is needed, which performs a higher-level abstraction based on the original player. 
On the one hand, it implements more complex logical control according to application scenarios; on the other hand, 
it further abstracts the player interface to improve developers' efficiency.
### Playback Control Protocol
The playback control component includes multiple sound types:

* System Sounds: Include system prompt tones and cloud-based TTS (Text-To-Speech) voices.
* Music: Encompasses songs played locally or from the cloud.
* Bluetooth Music: Refers to music played via Bluetooth connections.
* Bluetooth Calls: Includes incoming and outgoing phone calls via Bluetooth.

Smart Audio encapsulates the control layer (interfaces prefixed with smtaudio_ in smart_audio.c). 
Any playback type integrated into the Smart Audio playback control system must implement the following properties, 
which are invoked by the control layer to manage specific attributes of the playback type.

* const char *name;                                             |Sound source name
* int         prio;                                             |Sound source priority (the smaller the value, the higher the priority; the minimum value is 0)
* int         id;                                               |Sound source serial number
* msp_dlist_t     node;                                             |Sound source linked list node
* int         status;                                           |Sound source status (real-time updated status)
* int (*init)(void);                                            |Sound source initialization
* int (*deinit)(void);                                          |Sound source deinitialization
* int (*start)(const char *url, uint64_t seek_time, int resume);|Start playing the sound source
* int (*pause)(void);                                           |Pause the sound source playback
* int (*stop)(void);                                            |Stop the sound source playback
* int (*resume)(void);                                          |Resume the sound source playback
* int (*vol_get)(void);                                         |Get the current volume
* int (*vol_set)(int vol);                                      |Set the volume
* int (*vol_up)(int vol);                                       |Increase the volume
* int (*vol_down)(int vol);                                     |Decrease the volume
* void (*callback)(int type, smtaudio_player_evtid_t evt_id);   |Callback to notify the playback control when events such as start, pause, stop occur on the sound source

Each playback type has a priority (prio) attribute. In the module, Bluetooth calls have the highest priority (prio = 0), followed by system sounds (prio = 1), and finally Bluetooth music and network music (prio = 2). The specific manifestations are as follows:

* When a certain sound source is to be played, if a higher-priority sound source is currently playing, 
the sound source to be played will be temporarily added to the delay play list. After the higher-priority sound source finishes playing, it will be taken out of the delay play list and then played.
* When a certain sound source is to be played, if a lower-priority sound source is playing, the currently playing sound source will be interrupted, 
and the interrupted sound source will be added to the resume play list. Then the sound source to be played will start playing. 
After it finishes playing, the sound source will be taken out of the resume play list and its playback will be resumed.
* When music (Bluetooth or online music) is playing: if a system sound is to be played at this time, 
the music will be "interrupted" (music playback is paused) to give priority to playing the system sound; 
after the system sound finishes playing, a decision will be made on whether to resume music playback according to the situation.
* For sound sources with the same priority, the new sound source will interrupt the currently playing sound source, and the interrupted sound source cannot resume playback.

### Minimum Playback Volume

The playback control component incorporates a minimum playback volume design. This is because when users wake up the device with a wake word, 
it indicates their desire to engage in voice interaction with the device. Therefore, regardless of whether the current volume is set to 0, 
the user should be able to hear the device's response. When initializing the playback control component, the application layer needs to pass the minimum playback volume to it. 
When playing sounds, the component will check if the current volume is 0. If it is, the component will use the minimum playback volume to ensure the user can hear the sound.

## Component Installation
```bash
yoc init
yoc install smart_audio
```

## Configuration
None.

## Interface List

| Function | Description                               |
| :--- |:------------------------------------------|
| smtaudio_init | Initialize the playback control           |
| smtaudio_vol_up | Increase the volume                       |
| smtaudio_vol_down | Decrease the volume                       |
| smtaudio_vol_set | 	Set the volume to a fixed value          |
| smtaudio_vol_get | Get the current volume                    |
| smtaudio_pause | 	Pause music playback                     |
| smtaudio_resume | Resume music playback                     |
| smtaudio_mute | 	Enter/exit the mute state                |
| smtaudio_start      | 	Start playing an audio segment           |
| smtaudio_stop | Stop the current audio playback           |
| smtaudio_set_speed | Set the playback speed                    |
| smtaudio_get_state | Check the current playback control status |
| smtaudio_lpm | Enter/exit the low-power state            |

## Detailed Interface Description

### media_evt

`typedef void (*audio_evt_t)(int type, smtaudio_player_evtid_t evt_id);`

- Function Description:
   - Callback function for player event handling.


- Parameters:
   - `type`: Audio type
   - `evt_id`: Player event

- Return Value:
   - None.

### smtaudio_init

`int8_t smtaudio_init(audio_evt_t audio_evt_cb);`

- Function Description:
   - Initialize the playback control component.

- Parameters:
   - `audio_evt_cb`: Callback function for playback events provided to the user.

- Return Value:
   - 0: Initialization successful.
   - -1: Initialization failed.

### smtaudio_vol_up
`int8_t smtaudio_vol_up(int16_t vol);`

- Function Description:
   - Increase the volume.

- Parameters:
   - `vol`: The amount by which to increase the volume.

- Return Value:
   - 0: Setting successful.
   - -1: Setting failed.

### smtaudio_vol_down
`int8_t smtaudio_vol_down(int16_t vol);`

- Function Description:
   - Decrease the volume.

- Parameters:
   - `vol`: The amount by which to decrease the volume.

- Return Value:
   - 0: Setting successful.
   - -1: Setting failed.

### smtaudio_vol_set
`int8_t smtaudio_vol_set(int16_t vol);`

- Function Description:
   - Set the volume to a fixed value..

- Parameters:
   - `vol`: The volume level to be set.

- Return Value:
   - 0: Setting successful.
   - -1: Setting failed.

### smtaudio_vol_get
`int8_t smtaudio_vol_get(void);`

- Function Description:
   - Set the volume to a fixed value..

- Parameters:
   - None.

- Return Value:
   - The obtained volume value ranges from 0 to 100.

### smtaudio_pause
`int8_t smtaudio_pause(void);`

- Function Description:
   - Pause current playback.

- Parameters:
  - None.

- Return Value:
   - 0: Pause successful.
   - -1: Pause failed.

### smtaudio_resume
`int8_t smtaudio_resume(void);`

- Function Description:
   - Resume the paused music; if no music is currently paused, return directly.

- Parameters:
  - None.

- Return Value:
   - 0: Resume successful.
   - -1: Resume failed.

### smtaudio_mute
`int8_t smtaudio_mute(void);`

- Function Description:
   - Enter/exit the mute and mic-off state. The first call enters the mute and mic-off state, and the second call exits the mute and mic-off state.

- Parameters:
  - None.

- Return Value:
   - 0: Operation successful..
   - -1: Operation failed.

### smtaudio_start
`int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);`

- Function Description:
   - Start playing a piece of audio.

- Parameters:
   - `type`: Audio type.
   - `url`：Address of the audio content.
   - `seek_time`：Playback start position; 0 means starting from the beginning.
   - `resume`：Whether to resume the interrupted music after the playback ends.

- Return Value:
   - 0: Playback started successfully.
   - -1: Failed to start playback.

### smtaudio_stop
`int8_t smtaudio_stop(int type);`

- Function Description:
   - Stop the current audio playback.

- Parameters:
   - `type`: Audio type

- Return Value:
   - 0: Playback stopped successfully.
   - -1: Failed to stop playback.
### smtaudio_set_speed
`int smtaudio_set_speed(float speed)`

- Function Description:
   - Set the playback speed.

- Parameters:
   - `speed`: Playback speed, ranging from 0.5 to 2.0.

- Return Value:
   - 0: Setting successful.
   - -1: Setting failed.

### smtaudio_get_state
`smtaudio_state_t smtaudio_get_state(void);`

- Function Description:
   - Get the playback control status.

- Parameters:
   - None.

- Return Value:
   - The current playback control status is specified as follows:
```c
// Changes in the SYSTEM state will not alter the SMTAUDIO_STATE; only the MUSIC state will do so.
typedef enum {
    SMTAUDIO_STATE_PLAYING,      // Playing state
    SMTAUDIO_STATE_PAUSE,        // Paused state
    SMTAUDIO_STATE_MUTE,         // Mute and mic-off state; no response to other buttons or voice commands except the mic-off button
    SMTAUDIO_STATE_STOP,         // Stopped state
    SMTAUDIO_STATE_NUM
} smtaudio_state_t;
```

### smtaudio_lpm
`int8_t smtaudio_lpm(uint8_t state);`

- Function Description:
   - Enter/exit low-power state.

- Parameters:
   - `state`: Low-power state, 1 - enter low-power mode, 0 - exit low-power mode

- Return Value:
   - 0: Setting successful.
   - -1: Setting failed.

## Example

```c
// Callback function, which will be registered in the playback control component to receive playback events
static void media_evt(int type, smtaudio_player_evtid_t evt_id)
{
  switch (evt_id) {
        case SMTAUDIO_PLAYER_EVENT_START:
            /* Start playback event */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_ERROR:
            /* Playback error event */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_STOP:
            /* Playback end event */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_PAUSE:
            /* Playback pause event  */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_RESUME:
            /* Resume playback event */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_UNDER_RUN:
            /* Playback buffer data insufficient event */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_OVER_RUN:
            /* Playback buffer data overflow event */
            ...
            break;
        default:
            break;
    }
}

void main()
{
    ...;
    /* Initialize the playback control component */
    smtaudio_init(media_evt);
    ...
    /* Play the network stream using system sounds */
    smtaudio_start(MEDIA_SYSTEM,
                  "https://www.kozco.com/tech/LRMonoPhase4.mp3", 0, 1);
    ...
    /* Get the current status of the playback control component and control the volume. */
    if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
        smtaudio_vol_up(10);
    }
}
```

## Diagnostic Error Codes
None.

## Runtime Resources

None.

## Dependent Resources
  - csi
  - rhino
  - aos
  - kv
  - av

## Component Reference
None.