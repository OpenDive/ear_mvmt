# ear_mvmt

```
#!/bin/bash
# Publish a sequence of ear motions
ros2 topic pub /ear_motion_command std_msgs/Int32 "data: 1"  # Ear twitch
sleep 3
ros2 topic pub /ear_motion_command std_msgs/Int32 "data: 2"  # Ear wiggle
sleep 3
ros2 topic pub /ear_motion_command std_msgs/Int32 "data: 3"  # Ear sweep
sleep 3
ros2 topic pub /ear_motion_command std_msgs/Int32 "data: 8"  # Happy ears
```

```
ros2 topic echo /ear_motion_command
```

```
ros2 topic list | grep ear_motion_command
```
