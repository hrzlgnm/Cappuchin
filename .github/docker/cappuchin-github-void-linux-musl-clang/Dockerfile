FROM ghcr.io/void-linux/void-musl@sha256:c602cabbece5bfce9ac42bf972fd1448b1750a7580e05c47ab3c83670ddd3380
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash git cmake clang ninja clang clang-tools-extra cppcheck wget tar gzip jq \
    && xbps-remove -Ooy


