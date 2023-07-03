/*
* 클래스 상속, template, 예외처리 구현 - 23.6.13. 059분반 기말 코딩 시험
*/

#include <iostream>
#include <ctime>
#include <cstring>
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
class NotFoundException : public ExceptionError {
public:
	void Print() {
		cout << "Not found  예외 발생" << endl;
	}
};
class NotFoundRecordTableException : public NotFoundException {
public:
	void Print() {
		cout << "Not found record table 예외 발생" << endl;
	}
};
class NotFoundRowRecordsException : public NotFoundException {
public:
	void Print() {
		cout << "Not found record 예외 발생" << endl;
	}
};
class NotFoundUpdateRecordException : public NotFoundRecordTableException {
public:
	void Print() {
		NotFoundRecordTableException::Print();
		cout << "Not found update 예외 발생" << endl;
	}
};
class NotFoundDeleteRecordException : public NotFoundRecordTableException {
public:
	void Print() {
		NotFoundRecordTableException::Print();
		cout << "Not found delete 예외 발생" << endl;
	}
};
class Person {
	char* pid;
	char* pname;
public:
	Person() {}
	Person(char* pid, char* pname) :pid(pid), pname(pname) {}
	char* Key() { return pid; }
	virtual void Print();
};
void Person::Print() {
	cout << pid << ", " << pname;
}
class Employee : public Person {
private:
	char* eno;
	char* role;
public:
	Employee() {}
	Employee(char* pid, char* pname, char* eno, char* role) :Person(pid, pname), eno(eno), role(role) {}
	virtual void Print();
};

void Employee::Print() {
	Person::Print();
	cout << eno << ", " << role << endl;
}

class Student {
private:
	char* sid;
	char* sname;
public:
	Student() {}
	char* Key() { return sid; }
	Student(char* sid, char* sname) : sid(sid), sname(sname) {}
	virtual void Print();
};
void Student::Print() {
	cout << sid << ", " << sname;
}
class WorkStudent : public Student {
private:
	char* job;
public:
	WorkStudent() {}
	WorkStudent(char* sid, char* sname, char* job) : Student(sid, sname), job(job) {}
	void Print();
};

void WorkStudent::Print() {
	Student::Print();
	cout << job;
}
template <class T>
class RowRecords {
public:
	RowRecords() {}
	RowRecords(int maxKeys = 5, int unique = 1);// unique = 1은 중복이 없다
	~RowRecords();
	int Append(char* key, T* record);
	int Remove(const char* key, T*);
	T* Search(const char* key) const;
	void Print(ostream&) const;
	int numKeys()const { return NumKeys; }
	int Find(const char* key) const;
protected:
	int MaxKeys;
	int NumKeys;
	char** Keys;
	T** Records;

	int Init(const int maxKeys, const int unique);
	int Unique; // if true, each key value must be unique

};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique) : NumKeys(0), MaxKeys(maxKeys) {
	Init(MaxKeys, unique);
}
template <class T>
int RowRecords<T>::Init(const int maxKeys, const int unique) {
	Unique = unique != 0;//unique = 1이면 Unique는 true이고 중복이 없다
	MaxKeys = maxKeys;
	Records = new T * [MaxKeys];
	Keys = new char* [MaxKeys];
	return 1;
}
template <class T>
RowRecords<T>::~RowRecords() {
	delete[] Records;
	delete[] Keys;
}

//template <class T>
//int RowRecords<T>::Append(char* key, T* record) {
//	int i = NumKeys;
//	int index = Find(key);//current row에 중복인지를 check
//	if (Unique && index >= 0) return 0; //주어진 key가 이미 존재함
//	if (NumKeys == MaxKeys) throw RowRecordsOverflowException();
//
//	for (i = NumKeys - 1; i >= 0; i--) {
//		if (key > Keys[i]) break; // insert into location i+1
//		Keys[i + 1] = Keys[i];
//		Records[i + 1] = Records[i];
//	}
//
//	Keys[i] = strdup(key);//strdup()는 malloc()사용하여 스트링 사본에 대한 복제
//	Records[i] = record;
//	NumKeys++;
//	return 1;
//}
template <class T>
int RowRecords<T>::Append(char* key, T* record) {
	int i = NumKeys;
	int index = Find(key);
	if (Unique && index >= 0) return 0;
	if (NumKeys == MaxKeys) throw RowRecordsOverflowException();

	for (i = NumKeys - 1; i >= 0; i--) {
		if (std::strcmp(key, Keys[i]) > 0) break;  // strcmp 함수로 문자열 비교
		Keys[i + 1] = Keys[i];
		Records[i + 1] = Records[i];
	}

	Keys[i] = strdup(key);
	Records[i] = record;
	NumKeys++;
	return 1;
}


template <class T>
int RowRecords<T>::Remove(const char* key, T* record) {
	//주어진 RowRecords의 instance에 key가 있는지를 조사한 후에 제거
	int index = Find(key);
	if (index < 0) throw NotFoundRowRecordsException(); //key not in index
	//delete Records[index];//test시에 runtime error를 확인한다
	for (int i = index; i < NumKeys; i++) {
		Keys[i] = Keys[i + 1];
		Records[i] = Records[i + 1];
	}
	NumKeys--;
	return 1;
}

template <class T>
T* RowRecords<T>::Search(const char* key) const { //주어진 key를 갖고 있는 레코드를 리턴한다
	int index = Find(key);
	if (index != -1) return Records[index];
	throw NotFoundRowRecordsException();
}
template <class T>
void RowRecords<T>::Print(ostream& stream) const {
	for (int i = 0; i < NumKeys; i++) {
		// stream << endl; // << "\tKey[" << i << "] " << Keys[i] << " Records = ";
		stream << "\tKey[" << i << "] " << Keys[i] << " Records = " << endl;
		Records[i]->Print();
	}
}

//template <class T>
//int RowRecords<T>::Find(const char* key) const { //RecordTable의 주어진 row에 key가 있는지를 조사한다
//	for (int i = 0; i < NumKeys; i++) {
//		if (strcmp(Keys[i], key) == 0)
//			return i;//주어진 row에 key가 존재하고 index는 i 값을 리턴한다.
//	}
//	return -1; //주어진 row에 key가 없다
//}

template <class T>
int RowRecords<T>::Find(const char* key) const {
	try {
		for (int i = 0; i < NumKeys; i++) {
			if (std::strcmp(Keys[i], key) == 0) {
				return i;
			}
		}
	}
	catch (ExceptionError& e) {
		e.Print();  // 예외 메시지 출력
	}

	return -1;
}




template <class T>
class RecordTable {
public:
	T* Read(char* key);//read by key
	int Append(char* key, T* record);
	int Update(char* key, T* record);
	int Delete(char* key);
	void Print(ostream&) const;
	RecordTable(int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;
	int MaxRecords;
	int NumRecords;
	RowRecords<T>** data;
};

template <class T>
RecordTable<T>::RecordTable(int maxKeys, int maxRecords) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	data = new RowRecords<T>*[MaxRecords];

	for (int i = 0; i < MaxRecords; i++) {
		data[i] = new RowRecords<T>(MaxKeys);
	}
}

template <class T>
RecordTable<T>::~RecordTable() {
	delete[] data;
}

template <class T>
T* RecordTable<T>::Read(char* key) {
	for (int i = 0; i < NumRecords; i++) {
		try {
			int result = data[i]->Find(key);
			if (result >= 0) return data[i]->Search(key);
		}
		catch (EmptyException& e) {
			e.Print();
		}
	}
	throw NotFoundException();
}

template <class T>
int RecordTable<T>::Append(char* key, T* record) {
	int i = NumRecords;

	if (data[i]->numKeys() == MaxKeys) {
		i = ++NumRecords;
	}

	try {
		int result = data[i]->Append(key, record);
		if (result == 0) {
			throw RowRecordsOverflowException();
		}
		return result;
	}
	catch (OverflowException& e) {
		e.Print();
		return 0;
	}
	catch (RowRecordsOverflowException& e) {
		e.Print();
		return 0;
	}
}

template <class T>
int RecordTable<T>::Update(char* key, T* record) {//주어진 key를 갖는 record를 교체한다
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i]->Find(key);
		if (result >= 0) {
			try {
				data[i]->Remove(key, record);
				return data[i]->Append(key, record);
			}
			catch (RowRecordsOverflowException& e) {
				e.Print();
				return 0;
			}
		}
	}
	return 0;
}
template <class T>
int RecordTable<T>::Delete(char* key) {
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i]->Find(key);
		if (result >= 0) {
			try {
				return data[i]->Remove(key, data[i]->Search(key));
			}
			catch (EmptyException& e) {
				e.Print();
				return 0;
			}
		}
	}
	return 0;
}

template <class T>
void RecordTable<T>::Print(ostream& os) const {
	if (NumRecords == 0) {
		throw EmptyException();
	}

	for (int i = 0; i < NumRecords + 1; i++) {
		data[i]->Print(os);
	}
}

int main() {
	Person* members[30];
	Student* workers[20];
	RecordTable<Person> etable(10, 5);//10개의 record sets, 각 set은 5개의 records
	RecordTable<Student> wtable(10, 5);
	char* key;
	int select;
	Person* p; Student* s;
	int result;
	Person* pNew = new Employee("p1", "hongdada", "E111", "CPP Coding");
	Student* sNew = new WorkStudent("s026", "Bang", "전략영업");
	while (1)
	{
		cout << "\n선택 1: 객체 입력, 2.table 출력, 3: table 찾기,4. table 객체 삭제, 5. 객체 변경, 6. 종료" << endl;
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

			workers[0] = new WorkStudent("s011", "hong", "sales");
			workers[1] = new WorkStudent("s012", "ong", "designer");
			workers[2] = new WorkStudent("s013", "dong", "sales");
			workers[3] = new WorkStudent("s014", "fong", "회계");
			workers[4] = new WorkStudent("s015", "tong", "영업");
			workers[5] = new WorkStudent("s016", "nong", "designer");
			workers[6] = new WorkStudent("s017", "mong", "회계");
			workers[7] = new WorkStudent("s018", "kong", "sales");
			workers[8] = new WorkStudent("s019", "long", "영업");
			workers[9] = new WorkStudent("s020", "pong", "회계");
			workers[10] = new WorkStudent("s021", "lim", "영업");
			workers[11] = new WorkStudent("s022", "mim", "designer");
			workers[12] = new WorkStudent("s023", "bim", "sales");
			workers[13] = new WorkStudent("s024", "dim", "designer");
			workers[14] = new WorkStudent("s025", "rim", "회계");
			workers[15] = new WorkStudent("s026", "qim", "영업");
			workers[16] = new WorkStudent("s021", "fim", "sales");
			workers[17] = new WorkStudent("s021", "him", "회계");
			workers[18] = new WorkStudent("s027", "jim", "영업");
			workers[19] = new WorkStudent("s027", "jjj", "designer");

			try {
				for (int i = 0; i < 10; i++)
				{
					key = members[i]->Key();
					result = etable.Append(key, members[i]);

				}
				for (int i = 0; i < 20; i++)
				{
					key = workers[i]->Key();
					result = wtable.Append(key, workers[i]);

				}
			}
			catch (ExceptionError& e) {
				e.Print();
			}
			break;
		case 2:
			// table의 모든 객체 출력하기
			//table 모양으로 출력해야 한다: | *** | ???? | === |으로 출력한다.
			try {
				etable.Print(cout);
				wtable.Print(cout);
			}
			catch (ExceptionError& e) {
				e.Print();
			}
			break;
		case 3:
			// table에서 객체 찾기
			try {
				p = etable.Read("p6");
				p->Print();
				s = wtable.Read("s025");
				s->Print();
			}
			catch (ExceptionError& e) {
				e.Print();
			}
			break;
		case 4:
			//table에서 객체 삭제하기
			try {
				result = etable.Delete("p10");
				result = wtable.Delete("s027");
			}
			catch (ExceptionError& e) {
				e.Print();
			}
			break;
		case 5:
			try {
				result = etable.Update("p9", pNew);
				result = wtable.Update("s026", sNew);
			}
			catch (ExceptionError& e) {
				e.Print();
			}
			break;
		default:
			exit(0);

		}
	}
	system("pause");
	return 1;
}