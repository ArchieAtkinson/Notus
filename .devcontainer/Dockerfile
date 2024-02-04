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
ENV ZEPHYR_BASE=/home/user/zephyrproject/zephyr
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ENV APPLICATION_ROOT=$ZEPHYR_BASE/application

# Allow use of twister without long path
ENV PATH="${PATH}:${ZEPHYR_BASE}/scripts"

# Oh My Bash
RUN bash -c "$(wget https://raw.githubusercontent.com/ohmybash/oh-my-bash/master/tools/install.sh -O -)"

WORKDIR /home/user

# Install Zephyr
RUN west init --mr v${ZEPHYR_VERSION} zephyrproject

WORKDIR /home/user/zephyrproject

RUN west update && \
    west zephyr-export && \
    pip3 install --user -r zephyr/scripts/requirements.txt

WORKDIR /home/user/zephyrproject/zephyr

RUN mkdir application

CMD ["/bin/bash"]