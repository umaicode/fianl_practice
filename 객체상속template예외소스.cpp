/*
* 클래스 상속, template, 예외처리 구현
*/
#include <iostream>
#include <ctime>
using namespace std;
class ExceptionError {
public:
	virtual void Print() = 0;
};
class OverflowException : public ExceptionError {
public:
	void Print() {
		cout << "overflow 예외 발생" << endl;
	}
};
class RowRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Row Records overflow 예외 발생" << endl;
	}
};
class TableRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Table Records overflow 예외 발생" << endl;
	}
};
class EmptyException : public ExceptionError {
public:
	void Print() {
		cout << "empty 예외 발생" << endl;
	}
};
class NotFoundRecordException : public EmptyException {
public:
	void Print() {
		cout << "Not found record 예외 발생" << endl;
	}
};
class Person {
protected:
	string pid;
	string pname;
public:
	string GetName() { return pname; }
	void SetName(char* str) { pname = str; }
};

class Employee : public Person {
private:
	string eno;
	string role;
public:
	Employee() :Person() {}
	Employee(string pid, string pname, string eno, string role) : Person(pid, pname), eno(eno), role(role) { }

	virtual void Print();
	virtual bool Equals(Employee* p);
};
bool Employee::Equals(Employee* p) {
	return pid == p->pid && pname == p->pname && eno == p->eno && role == p->role;
}
void Employee::Print() {
	cout << pid << ", " << pname << ", " << eno << ", " << role;
}


class Student {
private:
	string sid;
	string major;
public:
	void Print();
};
void Student::Print() {
	cout << sid << ", " << major << ", ";
}

class WorkStudent : public Student {
private:
	string job;
public:
	void Print();
};

void WorkStudent::Print() {
	Student::Print();
	cout << job;
}

// 시험 때, RowRecord의 function은 제공할 것이다.
template <class T>
class RowRecords {
public:
	RowRecords(int maxKeys = 5, int unique = 1);// unique = 1은 중복이 없다
	~RowRecords();
	void Clear(); // RowRecords의 모든 레코드를 삭제한다
	int Insert(const string key, T& record);
	int Remove(const string key, const T&);
	T& Search(const string key) const;
	void Print(ostream&) const;
	int numKeys()const { return NumKeys; }
	int Find(const string key) const;
protected:
	int MaxKeys;
	int NumKeys;
	string* Keys;
	T* Records;

	int Init(const int maxKeys, const int unique);
	int Unique; // if true, each key value must be unique

};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique) : NumKeys(0), Keys(0), Records(0) {
	Init(unique);
}
template <class T>
int RowRecords<T>::Init(const int maxKeys, const int unique) {
	Unique = unique != 0;
	MaxKeys = maxKeys;
	Keys = new String[maxKeys];
	Records = new T[maxKeys];
}
template <class T>
RowRecords<T>::~RowRecords() {
	delete Keys;
	delete Records;
}
template <class T>
void RowRecords<T>::Clear() {
	NumKeys = 0;
}
template <class T>
int RowRecords<T>::Insert(const string key, T& record) {
	int i;
	int index = Find(key);
	if (Unique && index >= 0) return 0; //주어진 key가 이미 존재함
	if (NumKeys == MaxKeys) throw OverflowInsert();
	for (i = NumKeys - 1; i >= 0; i--) {
		if (key > Keys[i])break; //insert into location i+1
		// insert는 중간에 들어가는 것이다. 즉, a, b, c, d, e 중에 b에 넣고 싶다면 c, d, e를 한 칸씩 뒤로 밀어줘야 한다.
		// insert의 조건을 key값으로 걸 수도 있다.
		// 11, 22, 44, 55, 77 에서 33이 들어오려면 77은 다음 행으로 밀려야 한다. -> 여러운 point!
		// Key값으로 정렬하라 하면 어려워짐. -> 자료구조, 파일구조 => 시험에 안냄.
		// 배열의 순서로 정렬하려면 다 밀어내야한다. -> 효율x => 시험에 안냄.
		Keys[i + 1] = Keys[i];
		T[i + 1] = T[i];
	}
	Keys[i + 1] = key;
	T[i + 1] = record;
	NumKeys++;
	return 1;
}

// Record 삭제 할 때, delete 사용해야 한다. 객체를 main에서 new로 가져왔기 때문에 delete로 삭제해줘야 한다. -> runtime-error 발생 가능.
template <class T>
int RowRecords<T>::Remove(const string key, const T& record) {
	int index = Find(key, record);
	if (index < 0) return 0; //key not in index
	for (int i = index; i < NumKeys; i++) {
		Keys[i] = Keys[i + 1];
		T[i] = T[i + 1];
	}
	NumKeys--;
	return 1;
}

template <class T>
T& RowRecords<T>::Search(const string key) const {
	int index = Find(key);
	if (index != -1) return Records[index];
	throw EmptyException();
}
template <class T>
void RowRecords<T>::Print(ostream& stream) const {
	stream << "max keys = " << MaxKeys << " num keys = " << NumKeys << endl;
	for (int i = 0; i < NumKeys; i++) {
		stream << "\tKey[" << i << "] " << Keys[i] << " Records = " << Records[i] << endl;
	}
}

// find()는 현재 줄에서 원하는 key가 있는지 찾는 것
// 기존의 key가 있으면 입력 안한다.
// key가 없으면 입력한다.
// 현재 줄에서 key가 있는지 찾는 것이 밑에 만든 Find()
// 33을 넣으려고 하는데 만약 첫번째 줄에 33이 있다면 입력 안하겠다.
// -> 중복체크 조건이 시험에 나올 수 있다. (Key값이 있으면 안넣고, Key값이 없으면 넣겠다.)

template <class T>
int RowRecords<T>::Find(const string key) const {
	for (int i = 0; i < NumKeys; i++) {
		if (Keys[i] < key) continue;
		if (Keys[i] == key) {
			if (record == Records[i])
				return i;
		}
	}
	return -1;
}

template <class T>
class RecordTable {
public:
	T& Read(string  key);//read by key
	int Append(const T& record);
	int Update(string key, const T& record);
	int Delete(string key);
	void Print(ostream&) const;
	RecordTable(int keySize, int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;
	int MaxRecords;
	int NumRecords;
	RowRecords<T>* data;
};
template <class T>
RecordTable<T>::RecordTable(int keySize, int maxKeys = 5, int maxRecords = 10) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	for (int i = 0; i < MaxRecords; i++) {
		data[i] = new RowRecords<T>(maxKeys);
	}

}
template <class T>
RecordTable<T>::~RecordTable() {
	for (int i = 0; i < MaxRecords; i++) {
		delete data[i];
	}
}

// Read() : Key값이 주어지면 Record를 찾겠다.
// Find(), Search()를 이용하여 Read() 구현.
// RowRecords의 function들을 사용해서 구현.
// NotFoundRecord()를 던진다?, 예외객체를 던진다?
template <class T>
T& RecordTable<T>::Read(string key) {
	for (int i = 0; i < NumRecords; i++) {
		try {
			int result = data[i].Find(key);//data[i]는 row records이고 여기에 key가 있는지를 조사
		}
		catch () {

		}
		if (result > 0)
			return data[i].Search(key);//Search(key)를 사용하여 row records에서 해당 레코드를 가져온다

	}
	throw NotFoundRecord();
}
template <class T>
int RecordTable<T>::Append(const T& record) {
	string key = record.Key();
	int i = NumRecords;
	//NumRecords에 있는 row records가 append할 수 있는지를 조사한다.
	if (data[i].numKeys() == MaxKeys)
	{
		// 다음 행으로 넘기겠다.
		i = ++NumRecords;
	}
	// Append()를 Insert()로 구현.
	// 예외 발생 구간 : Insert() -> 한 줄이 꽉찼을 때. table이 다 찬 것은 아님.
	try {
		int result = data[i].Insert(record);
	}
	catch () {

	}

	return result;

}

// Record 바꿔치기 -> Remove(), Insert() 이용
template <class T>
int RecordTable<T>::Update(string key, const T& record) {
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i].Find(key);//data[i]는 row records이고 여기에 key가 있는지를 조사
		if (result > 0) {
			data[i].Remove(key, Search(key));
			data[i].Insert(key, record);
			return 1;
		}


	}
	return 0;
}

// delete를 하는데 해당 key를 가지고 있는 record가 없으면 예외 발생.
// table이 아무것도 없는데 삭제하려면 예외 발생
template <class T>
int RecordTable<T>::Delete(string key) {
}
template <class T>

// 맨 처음 빈테이블일 때는 출력할 것이 없기 때문에 예외 발생.
template <class T>
void RecordTable<T>::Print(ostream& os) const {
}

// RecordTable의 public 함수들!
// 입력 -> Append()
// 출력 -> Print()
// Read -> Read()
// 삭제 -> Delete()
// 변경 ->  update()
// 지난번이랑 다른 점 : 찾는데 Key를 사용한다.

// Person(pid, pname) <= Employee(eno, role) : 상속  ->  pid를 key로 만들 수도 있고, eno를 key로 만들 수도 있다.
// Student(sid, major) <= WorkStudent(job) : 상속  ->  sid를 key로 만들 수도 있고,  WorkStudent에서 String wsid, String job 이렇게 존재할 때, 둘 다 key가 될 수 있다.

// Table의 한 줄을 RowRecords라는 class로 정의
// 10줄이면 data[0] ~ data[9]
// 10줄이면 MaxRecords = 10
// MaxKeys = 열
// MaxRecords = 행
// NumRecords = top : 현재 입력하고 있는 줄
// 5개가 다 차면 NumRecords++ 해서 다음 줄로 가는 것이다.

// RowRecords 한 줄에서
// MaxKeys = 5 -> RowRecords 안에 집어 넣을 수 있는 데이터 수
// NumKeys = 열 -> 다 차면 다음 줄로 가는 것
// Keys = "hong"
// Records = "hong", "cse", "busan"
// Append()를 하게 되면 Keys와 Records가 같이 넣어져야 한다.
// ---------------------------------------------------------
// |	 Keys  | "hong" |			|		|		|		|
// ---------------------------------------------------------
// |	Records|		  |			|		|		|		|
// ---------------------------------------------------------

int main() {
	Employee* members[30];//Employee 선언으로 변경하는 문제 해결 필요 
	WorkStudent* workers[20];
	RecordTable<Employee> etable(10, 5);//10개의 record sets, 각 set은 5개의 records
	RecordTable<WorkStudent> wtable(10, 5);
	int select;
	Employee* e; WorkStudent* ws;
	int result;
	while (1)
	{
		cout << "\n선택 1: member 객체 30개 입력, 2: table 출력, 3: table 객체 찾기, 4: table에서 객체 삭제, 5: 변경, 6: 종료" << endl;
		cin >> select;
		switch (select) {
		case 1://table에 객체 30개 입력
			members[0] = new Employee("p1", "hong", "E1", "Coding");
			members[1] = new Employee("p2", "hee", "E2", "Coding");
			members[2] = new Employee("p3", "kim", "E3", "Test");
			members[3] = new Employee("p4", "lee", "E4", "Test");
			members[4] = new Employee("p5", "ko", "E5", "Design");
			members[5] = new Employee("p6", "choi", "E6", "Design");
			members[6] = new Employee("p7", "han", "E7", "Sales");
			members[7] = new Employee("p8", "na", "E8", "Sales");
			members[8] = new Employee("p9", "you", "E9", "Account");
			members[9] = new Employee("p10", "song", "E10", "Production");
			workers[0] = new WorkStudent("s011", "coding", "hong");
			workers[1] = new WorkStudent("s012", "coding", "ong");
			workers[2] = new WorkStudent("s013", "coding", "dong");
			workers[3] = new WorkStudent("s014", "coding", "fong");
			workers[4] = new WorkStudent("s015", "coding", "tong");
			workers[5] = new WorkStudent("s016", "coding", "nong");
			workers[6] = new WorkStudent("s017", "coding", "mong");
			workers[7] = new WorkStudent("s018", "coding", "kong");
			workers[8] = new WorkStudent("s019", "coding", "long");
			workers[9] = new WorkStudent("s020", "coding", "pong");
			workers[10] = new WorkStudent("s021", "coding", "lim");
			workers[11] = new WorkStudent("s022", "coding", "mim");
			workers[12] = new WorkStudent("s023", "coding", "bim");
			workers[13] = new WorkStudent("s024", "coding", "dim");
			workers[14] = new WorkStudent("s025", "coding", "rim");
			workers[15] = new WorkStudent("s026", "coding", "qim");
			workers[16] = new WorkStudent("s021", "coding", "fim");
			workers[17] = new WorkStudent("s021", "coding", "him");
			workers[18] = new WorkStudent("s027", "coding", "jim");
			workers[19] = new WorkStudent("s027", "coding", "jjj");

			try {
				for (int i = 0; i < 10; i++)
				{
					// tableoverflow 는 여기서 catch
					// 한 줄이 overflow되면 main에서 catch 할 필요 없다.
					// for loop에서 try ~ catch 하면 필요한 조치하고 continue -> 또 돈다.
					// 바깥에서 catch하면 for loop 중단
					// try {
					etable.Append(members[i]);
					//}
					//catch () {

					//}

				}
				for (int i = 0; i < 20; i++)
				{
					wtable.Append(workers[i]);

				}
			}

			catch () {

			}
			break;
		case 2:
			// table의 모든 객체 출력하기
			//table 모양으로 출력해야 한다: | *** | ???? | === |으로 출력한다.

			etable.Print();
			wtable.Print();
			break;
		case 3:
			// table에서 객체 찾기
			try {
				e = etable.Read("kim");
				if (e == nullptr) cout << "kim이 존재하지 않는다" << endl;
				else
					e->Print();
			}
			catch () {

			}


			ws = wtable.Read("kim");
			if (ws == nullptr) cout << "kim이 존재하지 않는다" << endl;
			else
				ws->Print();
			break;
		case 4:
			//table에서 객체 삭제하기
			result = etable.Delete("hong");
			if (result > 0)
				cout << "삭제된 records 숫자" << result << endl;
			result = wtable.Delete("hong");
			if (result > 0)
				cout << "삭제된 records 숫자" << result << endl;
			break;
		case 5:
			result = etable.Update("hong", *e);
			if (result > 0)
				cout << "삭제된 records 숫자" << result << endl;
			result = wtable.Update("hong", *ws);
			if (result > 0)
				cout << "삭제된 records 숫자" << result << endl;
			break;

			// main에서 try ~ catch하는 방법
			//try {
			//	result = etable.Update("hong", *e);
			//	if (result > 0)
			//		cout << "삭제된 records 숫자" << result << endl;
			//	result = wtable.Update("hong", *ws);
			//}
			//catch () {

			//}

			//if (result > 0)
			//	cout << "삭제된 records 숫자" << result << endl;
			//break;
		default:
			exit(0);

		}
	}
	system("pause");
	return 1;
}