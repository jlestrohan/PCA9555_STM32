# PCA9555_STM32
TI PCA9555 driver files for STM32

This work is actually under construction, the driver "may" be bugged and will be fixed when I have some time.
I put this here as a "work to be done" kindof stuff, and eventually to anyone who wish to finish working on this.

I was planning to use this chip on a project, and wrote a driver, but the way it handles the IRQ (very simplistic) doesn't make the job to me as it would force polling everytime an IRQ has been raised, with no real way to get the last pin that raised the interrupt. On a 16 I/O chip, the lost processor cycles polling to try to guess which pin has changed is not acceptable. So I choose to stay on two 23017 (one in one out).

The PCA9555 is a very good multiplexer and may fit well in many projects, and the present drivers files are very close to fully work but I have no time to check and test for now so if you find any issues please bear with them for the moment, or even better feel free to contribute I will gladly merge any pull requests that fix the eventual issues encountered here.

ps: not using any kind of union to split uint16_t into 2xuint8_t like I saw in many other drivers files around here (arduino especially), let's stay efficient and clean ;) bits and shifts like to be played with too without adding unecessary overwhelming code on top of it. ;)

