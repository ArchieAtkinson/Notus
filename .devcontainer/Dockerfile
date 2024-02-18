FROM zephyrprojectrtos/ci:v0.26.6

ARG ZEPHYR_VERSION=3.5.0

# Adjust 'user' home directory permissions
USER root

RUN chown -R user:user /home/user

RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x llvm.sh
RUN sudo ./llvm.sh 17 all

# Switch to 'user' context
USER user

# Configure environment variables
ENV DISPLAY=:0
ENV WEST_WORKSPACE=/home/user/west-workspace
ENV ZEPHYR_BASE=$WEST_WORKSPACE/zephyr
ENV APPLICATION_ROOT=$WEST_WORKSPACE/application
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr

# Allow use of twister without long path
ENV PATH="${PATH}:${ZEPHYR_BASE}/scripts"

# Oh My Bash
RUN bash -c "$(wget https://raw.githubusercontent.com/ohmybash/oh-my-bash/master/tools/install.sh -O -)"

RUN mkdir -p $APPLICATION_ROOT

WORKDIR $APPLICATION_ROOT

CMD ["/bin/bash"]