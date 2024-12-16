FROM gcc:12

COPY . /app/
COPY *.cpp /app/
COPY *.hpp /app/
COPY *.h /app/
COPY *.json /app/

WORKDIR /app

RUN g++ -std=c++17 -o server *.cpp -lpthread -lssl -lcrypto

CMD ["./server"]
