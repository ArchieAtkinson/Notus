FROM zephyrprojectrtos/ci:v0.26.6

ENV NRF_CLI_VERSION=10.24.0
ENV LLVM_VERSION=17

# Adjust 'user' home directory permissions
USER root

RUN chown -R user:user /home/user

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    sudo ./llvm.sh ${LLVM_VERSION} all

RUN sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION} 10 && \
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 10


RUN VERSION_DASHES=$(echo ${NRF_CLI_VERSION} | tr '.' '-') && \
    wget https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/desktop-software/nrf-command-line-tools/sw/versions-10-x-x/$VERSION_DASHES/nrf-command-line-tools_${NRF_CLI_VERSION}_amd64.deb && \
    sudo apt-get install -y ./nrf-command-line-tools_${NRF_CLI_VERSION}_amd64.deb && \
    sudo rm ./nrf-command-line-tools_${NRF_CLI_VERSION}_amd64.deb && \
    mkdir jlink_unpacked && \
    dpkg-deb -R /opt/nrf-command-line-tools/share/JLink*.deb jlink_unpacked && \
    rm jlink_unpacked/DEBIAN/postinst && \
    dpkg-deb -b jlink_unpacked jlink_fixed.deb && \
    apt-get install -y ./jlink_fixed.deb

RUN apt-get install -y \
    cppcheck \
    minicom

RUN pip install codechecker

ENV CLANGD_TIDY_DIR=/opt/clangd-tidy
RUN sudo git clone -b pretty-output https://github.com/ArchieAtkinson/clangd-tidy.git ${CLANGD_TIDY_DIR}

ENV PATH="${PATH}:${CLANGD_TIDY_DIR}"

# Switch to 'user' context
USER user

# Configure environment variables
ENV DISPLAY=:0
ENV WEST_WORKSPACE=/home/user/west-workspace
ENV ZEPHYR_BASE=$WEST_WORKSPACE/zephyr
ENV NOTUS_ROOT=$WEST_WORKSPACE/notus
ENV COFFEE_SCALE_ROOT=$NOTUS_ROOT/applications/coffee_scale
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr

# Allow use of twister without long path
ENV PATH="${PATH}:${ZEPHYR_BASE}/scripts"

# Oh My Bash
RUN bash -c "$(wget https://raw.githubusercontent.com/ohmybash/oh-my-bash/master/tools/install.sh -O -)"

RUN mkdir -p $NOTUS_ROOT

WORKDIR $NOTUS_ROOT

COPY west.yml .
COPY scripts/install.sh . 

RUN ./install.sh && \
    rm install.sh && \
    rm west.yml

USER root


CMD ["/bin/bash"]