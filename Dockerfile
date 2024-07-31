FROM alpine AS build

RUN apk update && apk add cmake make gcc g++ cppzmq spdlog-dev
WORKDIR /opt/build
COPY . .
RUN cmake . && cmake --build .

FROM alpine

RUN apk update && apk add spdlog libzmq libgcc libstdc++
COPY --from=build /opt/build/0c-push-pull /opt/0c/
COPY --from=build /opt/build/0c-pub-sub /opt/0c/

ENV PATH="$PATH:/opt/0c"
EXPOSE 3000
EXPOSE 4000
HEALTHCHECK --start-period=30s --start-interval=1s CMD pgrep "0c"
CMD ["0c-push-pull"]
