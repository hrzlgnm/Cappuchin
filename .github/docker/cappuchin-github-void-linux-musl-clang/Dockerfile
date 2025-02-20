FROM ghcr.io/void-linux/void-musl@sha256:d67fe7877006d5ef9c3f34cba94baaab34e6a38f2f6a03c5577f17840b9554ac
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash git cmake clang ninja clang clang-tools-extra cppcheck wget tar gzip jq \
    && xbps-remove -Ooy


