FROM ghcr.io/void-linux/void-glibc@sha256:d1008b3b77371fc60bb32f40cd2553399f933ec61a388bbaed5636afe7fb6e94
RUN xbps-install -Syu || xbps-install -yu xbps \
    && xbps-install -yu \
    && xbps-install -y bash git cmake ninja gcc libsanitizer-devel make \
    && xbps-remove -Ooy


