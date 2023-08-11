# Newlib Syscalls

## Newlib configurations

| CONFIG                 |  Description                |
|:----------------------:|:---------------------------:|
| CONFIG_NEWLIB          |  Enable newlib support      |
| CONFIG_NEWLIB_STANDARD |  Enable newlib full standart support (default nano) |
| CONFIG_NEWLIB_FATFS    |  Enable newlib filesystem port fatfs support |

## printf and scanf

| CONFIG_VSNPRINTF_NANO | CONFIG_NEWLIB | CONFIG_NEWLIB_STANDARD | PRINTF         | SCANF       | stdlib | string | other libc  |
|:---------------------:|:-------------:|:----------------------:|:--------------:|:-----------:|:------:|:------:|:-----------:|
| false                 | false         | false                  | vsnprintf      | newlib.nano | nuttx  | nuttx  | newlib.nano |
| true                  | false         | false                  | vsnprintf.nano | newlib.nano | nuttx  | nuttx  | newlib.nano |
| any                   | true          | false                  | newlib.nano    | newlib.nano | nuttx  | nuttx  | newlib.nano |
| any                   | true          | true                   | newlib         | newlib      | newlib | newlib | newlib      |

Other configurations are illegal !!!

## printf configurations

| CONFIG_VSNPRINTF_FLOAT | vsnprintf | vsnprintf.nano | newlib.nano       | newlib         |
|:----------------------:|:---------:|:--------------:|:-----------------:|:--------------:|
| false                  | none      | none           | none              | all c99 format |
| true or not set        | %f %F     | none           | %f %F %g %G %e %E | all c99 format |

| CONFIG_VSNPRINTF_FLOAT_EX | vsnprintf   | vsnprintf.nano | newlib.nano       | newlib         |
|:-------------------------:|:-----------:|:--------------:|:-----------------:|:--------------:|
| false                     | none        | none           | none              | all c99 format |
| true or not set           | %g %G %e %E | none           | %f %F %g %G %e %E | all c99 format |

| CONFIG_VSNPRINTF_LONG_LONG | vsnprintf   | vsnprintf.nano | newlib.nano | newlib         |
|:--------------------------:|:-----------:|:--------------:|:-----------:|:--------------:|
| false                      | none        | none           | none        | all c99 format |
| true or not set            | support     | support        | none        | all c99 format |

## scanf configurations

scanf always support regular match.

| CONFIG_SCANF_FLOAT | newlib.nano       | newlib         |
|:------------------:|:-----------------:|:--------------:|
| false              | none              | all c99 format |
| true or not set    | %f %F %g %G %e %E | all c99 format |

| CONFIG_SCANF_FLOAT_EX | newlib.nano       | newlib         |
|:---------------------:|:-----------------:|:--------------:|
| false                 | none              | all c99 format |
| true or not set       | %f %F %g %G %e %E | all c99 format |

| CONFIG_SCANF_LONG_LONG | newlib.nano | newlib         |
|:----------------------:|:-----------:|:--------------:|
| false                  | none        | all c99 format |
| true or not set        | none        | all c99 format |