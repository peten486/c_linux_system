TCP 프로그래밍

TCP(Transmission control Protocol)
----------------------------------
* 신뢰성 있는 Byte-stream으로 된 Virtual Circuit Protocol
* Connection Oriented Protocol

+ 프로세스간 통신
+ 연결성립과 연결 종료
+ 신뢰성 있는 Packet전송
+ 연속되는 Packet전송
+ Data Overflow 방지를 위한 Flow Control
+ Error Control
+ Multiplexing과 Demultiplexing
+ Full Duplexing Connection

TCP Segment Format
------------------
* TCP를 이용하여 두 개의 장치간에 전달되는 데이터 단위
* 20Byte의 고정 헤더와 40Byte까지의 가변 옵션
* 제어 필드
  * URG : Urgent pointer is valid
  * ACK : Acknowledgment is valid
  * PSH : Request for push
  * RST : Reset the connection
  * SYN : Synchronize sequence numbers
  * FIN : Terminate teh connection
의 순서로 제어필드가 구성되어 있음

TCP 3way Handshake
------------------

TCP 4way Handshake
------------------

Data Transmission
-----------------
* 수신버퍼의 Overflow 해소
* 윈도우 크기는 목적지에 의해 가변적으로 변한다.

TCP Flow Control
----------------
* 송신 패킷에 대한 확인응답을 받는다.
* 패킷 송신에 대한 확인응답 수신의 비효율성 해소

Error Control
-------------
* TCP는 오류제어를 통하여 신뢰성을 제공
* 오류 감지 및 정정을 위한 매커니즘 제공
  * 오류상황
    - 손상 세그먼트
    - 손실 세그먼트
    - 순서가 어긋난 세그먼트
    - 중복 세그먼트
  * 오류 발견과 정정
    - check sum
    - 확인응답
    - 시간초과

TCP Timer
---------
* Retransmission : 시간초과(기다리고 있는 시간)
* Persistence 
* Keepalive : 살아있는지 확인
* Time-waited : FIN 메시지 기다림

