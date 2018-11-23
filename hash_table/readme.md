1. hash table의 구조 : m개의 bucket, s개의 slot
2. hash func : 구현
3. 다수의 key(string), value (string) 입력 받아 hash_insert 
4. key 만 입력 받아 hash search 
4.1 있는 경우 출력 후 hash delete  
4.2 없는 경우 출력 후 hash insert, insert 시 해당 bucket의 slot이 모두 차있다면 에러 출력 


1 2 3 .........
ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ
ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ
ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ
ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ