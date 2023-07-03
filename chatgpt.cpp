#include <iostream>
#include <string>

using namespace std;

class ExceptionError {
public:
	virtual void Print() = 0;
	virtual ~ExceptionError() {}
};

class OverflowException : public ExceptionError {
public:
	void Print() {
		cout << "Overflow 예외 발생" << endl;
	}
};

class RowRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Row Records Overflow 예외 발생" << endl;
	}
};

class TableRecordsOverflowException : public OverflowException {
public:
	void Print() {
		cout << "Table Records Overflow 예외 발생" << endl;
	}
};

class EmptyException : public ExceptionError {
public:
	void Print() {
		cout << "Empty 예외 발생" << endl;
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
	Person() {}
	Person(string pid, string pname) : pid(pid), pname(pname) {}
	string GetName() { return pname; }
	void SetName(string str) { pname = str; }
	string Key() { return pname; }
};

class Employee : public Person {
private:
	string eno;
	string role;
public:
	Employee() : Person() {}
	Employee(string pid, string pname, string eno, string role) : Person(pid, pname), eno(eno), role(role) {}

	virtual void Print();
	virtual bool Equals(Employee* p);
};

bool Employee::Equals(Employee* p) {
	return pid == p->pid && pname == p->pname && eno == p->eno && role == p->role;
}

void Employee::Print() {
	cout << pid << ", " << pname << ", " << eno << ", " << role << endl;
}

class Student {
private:
	string sid;
	string major;
public:
	Student() {}
	Student(string sid, string major) : sid(sid), major(major) {}
	void Print();
};

void Student::Print() {
	cout << sid << ", " << major << ", ";
}

class WorkStudent : public Student {
private:
	string job;
public:
	WorkStudent() {}
	WorkStudent(string sid, string major, string job) : Student(sid, major), job(job) {}
	void Print();
	string Key() { return job; }
};

void WorkStudent::Print() {
	Student::Print();
	cout << job << endl;
}

template <class T>
class RowRecords {
public:
	RowRecords(int maxKeys = 5, int unique = 1);
	~RowRecords();
	void Clear();
	int Insert(const string key, const T& record);
	int Remove(const string key, const T& record);
	T& Search(const string key) const;
	void Print(ostream& os) const;
	int numKeys() const { return NumKeys; }
	int Find(const string key) const;
protected:
	int MaxKeys;
	int NumKeys;
	string* Keys;
	T* Records;
	int Init(const int maxKeys, const int unique);
	int Unique;
};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique) : NumKeys(0) {
	Init(maxKeys, unique);
}

template <class T>
int RowRecords<T>::Init(const int maxKeys, const int unique) {
	Unique = unique != 0;
	MaxKeys = maxKeys;
	Keys = new string[MaxKeys];
	Records = new T[MaxKeys];
	return 0;
}

template <class T>
RowRecords<T>::~RowRecords() {
	delete[] Keys;
	delete[] Records;
}

template <class T>
void RowRecords<T>::Clear() {
	NumKeys = 0;
}

template <class T>
int RowRecords<T>::Insert(const string key, const T& record) {
	int index = Find(key);
	if (Unique && index >= 0) return 0; // 주어진 key가 이미 존재함
	if (NumKeys == MaxKeys) throw RowRecordsOverflowException(); // RowRecords가 다 찼을 때 예외 처리

	int i;
	for (i = NumKeys - 1; i >= 0; i--) {
		if (key > Keys[i]) break; // insert into location i+1
		Keys[i + 1] = Keys[i];
		Records[i + 1] = Records[i];
	}

	Keys[i + 1] = key;
	Records[i + 1] = record;
	NumKeys++;
	return 1;
}

template <class T>
int RowRecords<T>::Remove(const string key, const T& record) {
	int index = Find(key);
	if (index < 0) return 0;

	for (int i = index; i < NumKeys; i++) {
		Keys[i] = Keys[i + 1];
		Records[i] = Records[i + 1];
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
void RowRecords<T>::Print(ostream& os) const {
	if (NumKeys == 0) {
		throw EmptyException();
	}

	os << "max keys = " << MaxKeys << " num keys = " << NumKeys << endl;
	for (int i = 0; i < NumKeys; i++) {
		os << "\tKey[" << i << "] " << Keys[i] << " Records = ";
		Records[i].Print();
	}
}

template <class T>
int RowRecords<T>::Find(const string key) const {
	for (int i = 0; i < NumKeys; i++) {
		if (Keys[i] < key) continue;
		if (Keys[i] == key) return i;
	}
	return -1;
}

template <class T>
class RecordTable {
public:
	T& Read(string key);
	int Append(T& record);
	int Update(string key, T& record);
	int Delete(string key);
	void Print(ostream& os) const;
	RecordTable(int keySize, int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;
	int MaxRecords;
	int NumRecords;
	RowRecords<T>* data;
};

template <class T>
RecordTable<T>::RecordTable(int keySize, int maxKeys, int maxRecords) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	data = new RowRecords<T>[MaxRecords];
}

template <class T>
RecordTable<T>::~RecordTable() {
	delete[] data;
}

template <class T>
T& RecordTable<T>::Read(string key) {
	for (int i = 0; i < NumRecords; i++) {
		try {
			int result = data[i].Find(key);
			if (result >= 0) return data[i].Search(key);
		}
		catch (EmptyException& e) {
			e.Print();
		}
	}
	throw NotFoundRecordException();
}

template <class T>
int RecordTable<T>::Append(T& record) {
	string key = record.Key();
	int i = NumRecords;

	if (data[i].numKeys() == MaxKeys) {
		i = ++NumRecords;
	}

	try {
		int result = data[i].Insert(key, record);
		return result;
	}
	catch (OverflowException& e) {
		e.Print();
		return 0;
	}
}

template <class T>
int RecordTable<T>::Update(string key, T& record) {
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i].Find(key);
		if (result >= 0) {
			try {
				data[i].Remove(key, record);
				return data[i].Insert(key, record);
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
int RecordTable<T>::Delete(string key) {
	for (int i = 0; i < NumRecords; i++) {
		int result = data[i].Find(key);
		if (result >= 0) {
			try {
				return data[i].Remove(key, data[i].Search(key));
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
		data[i].Print(os);
	}
}

int main() {
	Employee* members[30];
	WorkStudent* workers[20];
	RecordTable<Employee> etable(10, 5);
	RecordTable<WorkStudent> wtable(10, 5);
	int select;
	Employee* e;
	WorkStudent* ws;
	int result;

	while (1) {
		cout << "\n선택 1: member 객체 30개 입력, 2: table 출력, 3: table 객체 찾기, 4: table에서 객체 삭제, 5: 변경, 6: 종료" << endl;
		cin >> select;

		switch (select) {
		case 1:
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
				for (int i = 0; i < 10; i++) {
					result = etable.Append(*members[i]);
					if (result == 0) {
						cout << "Employee 객체 추가 실패" << endl;
					}
				}
			}
			catch (ExceptionError& e) {
				e.Print();
			}

			try {
				for (int i = 0; i < 20; i++) {
					result = wtable.Append(*workers[i]);
					if (result == 0) {
						cout << "WorkStudent 객체 추가 실패" << endl;
					}
				}
			}
			catch (ExceptionError& e) {
				e.Print();
			}

			break;

		case 2:
			try {
				etable.Print(cout);
			}
			catch (EmptyException& e) {
				e.Print();
			}

			try {
				wtable.Print(cout);
			}
			catch (EmptyException& e) {
				e.Print();
			}
			break;

		case 3:
			try {
				e = &etable.Read("kim");
				if (e == nullptr) {
					cout << "kim이 존재하지 않습니다." << endl;
				}
				else {
					e->Print();
				}
			}
			catch (NotFoundRecordException& e) {
				e.Print();
			}

			try {
				ws = &wtable.Read("kim");
				if (ws == nullptr) {
					cout << "kim이 존재하지 않습니다." << endl;
				}
				else {
					ws->Print();
				}
			}
			catch (NotFoundRecordException& e) {
				e.Print();
			}

			break;

		case 4:
			try {
				// table에서 객체 삭제하기
				result = etable.Delete("hong");
				if (result > 0)
					cout << "삭제된 records 숫자: " << result << endl;
				result = wtable.Delete("hong");
				if (result > 0)
					cout << "삭제된 records 숫자: " << result << endl;
			}
			catch (EmptyException& e) {
				e.Print();
			}
			break;

		case 5:
			try {
				// 변경하기
				result = etable.Update("hong", *e);
				if (result > 0)
					cout << "변경된 records 숫자: " << result << endl;
				else
					cout << "변경 실패: hong을 찾을 수 없습니다." << endl;

				result = wtable.Update("hong", *ws);
				if (result > 0)
					cout << "변경된 records 숫자: " << result << endl;
				else
					cout << "변경 실패: hong을 찾을 수 없습니다." << endl;
			}
			catch (RowRecordsOverflowException& e) {
				e.Print();
			}
			catch (EmptyException& e) {
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
