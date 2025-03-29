# cappuchin-github-void-linux-glibc-clang Docker image

## Updating the Docker image

> [!NOTE]
> This is usually not needed to be done manually, as the Docker image is automatically built and pushed
> by the GitHub Actions workflow defined in the `.github/workflows/docker-build.yml` file.

Pick a version number for the new Docker image (e.g. `v2`), then run the
following commands:

    $ docker build --tag ghcr.io/hrzlgnm/cappuchin-github-void-linux-glibc-clang:VERSION_NUMBER_HERE .github/docker/cappuchin-github-void-linux-glibc-clang/
    $ docker login ghcr.io -u YOUR_GITHUB_USER_NAME_HERE
    $ docker push ghcr.io/hrzlgnm/cappuchin-github-void-linux-glibc-clang:VERSION_NUMBER_HERE

Then, change the container tag in each workflow file in the .github/workflows/
directory to refer to your new version.
