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
	string pid;
	string pname;
public:
	Person() {}
	Person(string pid, string pname) :pid(pid), pname(pname) { }
	string GetName() { return pname; }
	void SetName(char* str) { pname = str; }
	string GetId() { return pid; }
	virtual ~Person() {}
};

class Employee : public Person {
private:
	string eno;
	string role;
public:
	Employee() :Person() {}
	Employee(string pid, string pname, string eno, string role) :Person(pid, pname), eno(eno), role(role) {}
	virtual void Print();
	virtual bool Equals(Employee* p);
	string Key() { return GetName(); }
};
bool Employee::Equals(Employee* p) {
	return (eno == p->eno && role == p->role &&
		this->GetName() == p->GetName() && this->GetId() == p->GetId());
}
void Employee::Print() {
	cout << " eno: " << eno << " role: " <<
		role << " pid: " << GetId() << " pname: " << GetName() << endl;
}


class Student {
private:
	string sid;
	string major;
public:
	virtual void Print();
	Student() {}
	Student(string sid, string major) : sid(sid), major(major) {}
	string getSid() { return sid; }
	string getMajor() { return major; }

};
void Student::Print() {

	cout << " sid: " << sid << " major: " << major;
}
class WorkStudent : public Student {
private:
	string job;
public:
	WorkStudent() { Student(); }
	WorkStudent(string sid, string major, string job) :Student(sid, major), job(job) {}
	bool Equals(WorkStudent* p);
	void Print();
	string Key() { return job; }
};

bool WorkStudent::Equals(WorkStudent* p) {
	return (getSid() == p->getSid() && getMajor() == p->getMajor() && job == p->job);
}

void WorkStudent::Print() {
	Student::Print();
	cout << " job: " << job << endl;
}

template <class T>
class RowRecords {//이 클래스의 메서드에서 파라미터는 좀 더 다듬어서 출제 할 예정
public:
	RowRecords(int maxKeys, int unique = 1);// unique = 1은 중복이 없다
	~RowRecords();
	void Clear(); // RowRecords의 모든 레코드를 삭제한다//쓸일 없을 듯
	//아래의 6개 메서드를 이용해서 구현,이건 구현 해줄거임
	int Insert(const string key, T& record);//
	int Remove(const string key);
	T& Search(const string key) const;
	void Print(ostream&) const;
	int numKeys()const { return NumKeys; }
	int Find(const string key) const;
protected:
	int MaxKeys;//5개
	int NumKeys;//top 역할 현재 넣고 있는 줄 넣고 나서++
	string* Keys;//key가 MaxKeys를 넘게 가지고 있을 수는 없음
	T* Records;//Keys개수와 같아야함
	int DeleteNumber;

	//int Init(int maxKeys, int unique);
	int Init(int unique);
	int Unique; // if true, each key value must be unique

};

template <class T>
RowRecords<T>::RowRecords(int maxKeys, int unique = 1) : NumKeys(0) {//maxKeys=10을 입력받음
	MaxKeys = maxKeys;//5
	Init(unique);//1
}
template <class T>
int RowRecords<T>::Init(int unique) {
	Unique = unique != 0;
	//MaxKeys = maxKeys;
	Keys = new string[MaxKeys];
	Records = new T[MaxKeys];
	DeleteNumber = 0;
	return 1;
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
int RowRecords<T>::Insert(const string key, T& record) {//append에서 불러와지고 그냥 가장 뒤에 붙이기
	int i;
	int index = Find(key);
	if (Unique && index >= 0) return 0; //주어진 key가 이미 존재함
	if (NumKeys == MaxKeys) throw RowRecordsOverflowException();
	for (i = NumKeys - 1; i >= 0; i--) {//키값 가지고 이름 순으로 정렬//다음 줄로 넘어갈 때가 힘듦//키값 정렬 하지말자--->이거하면 2/3이 나가 떨어짐//
		if (key > Keys[i])break; //insert into location i+1
		Keys[i + 1] = Keys[i];
		Records[i + 1] = Records[i];
	}
	Keys[i + 1] = key;
	Records[i + 1] = record;
	NumKeys++;
	cout << "NumKeys: " << NumKeys << endl;
	return 1;
}

template <class T>
int RowRecords<T>::Remove(const string key) {//C++ 에서는 레코드 삭제할 때 Delete 해줘야 함
	int index = Find(key);
	if (index < 0) return 0; //key not in index
	for (int i = index; i < NumKeys - 1; i++) {
		Keys[i] = Keys[i + 1];//Delete를 안하고 앞으로 데이터를 옮기기만 하면 runTimeError이 뜰거임
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
void RowRecords<T>::Print(ostream& stream) const {
	stream << "max keys = " << MaxKeys << " num keys = " << NumKeys << endl;

	for (int i = 0; i < NumKeys; i++) {
		stream << "\tKey[" << i << "] " << Keys[i] << " Records = ";
		Records[i].Print();
	}
}

template <class T>
int RowRecords<T>::Find(const string key) const {//이미 그 키가 있으면 입력하지 않기 위해서 중복안되게 하기 위한 메서드
	for (int i = 0; i < NumKeys; i++) {
		if (Keys[i] < key) continue;
		if (Keys[i] == key) {
			return i;
		}
	}
	return -1;
}

template <class T>
class RecordTable {
public:
	T& Read(string  key);//read by key
	int Append(T& record);
	int Update(string key, const T& record);
	int Delete(string key);
	void Print(ostream&) const;
	RecordTable(int maxKeys = 5, int maxRecords = 10);
	~RecordTable();
protected:
	int MaxKeys;//행
	int MaxRecords;//열
	int NumRecords;//현재 입력하고 있는 줄 top 한 줄 다 채우면 ++하기
	RowRecords<T>** data;//한 줄이 RowRecords로 정의 10줄이면 data[0] ~ data[9]
};

template <class T>
RecordTable<T>::RecordTable(int maxKeys, int maxRecords) : MaxKeys(maxKeys), MaxRecords(maxRecords) {
	NumRecords = 0;
	data = new RowRecords<T> *[MaxKeys];
	for (int i = 0; i < MaxKeys; i++) {//5번 반복
		data[i] = new RowRecords<T>(MaxRecords);//10개 만듦
	}

}
//template <class T>
//RecordTable<T>::~RecordTable() {
//   for (int i = 0; i < MaxRecords; i++) {
//      delete data[i];
//   }
//}

template <class T>
RecordTable<T>::~RecordTable() {
	delete[] data;
}

template <class T>
T& RecordTable<T>::Read(string key) {
	for (int i = 0; i < NumRecords; i++) {//각줄에서 key를 찾기위해 탐색
		try {
			int result = data[i]->Find(key);//data[i]는 row records이고 여기에 key가 있는지를 조사
			if (result > 0)
				return data[i]->Search(key);//Search(key)를 사용하여 row records에서 해당 레코드를 가져온다
		}

		catch (ExceptionError& error) {
			error.Print();
		}

	}
	throw NotFoundRecordException();//nullptr return하지말고 이걸로 예외처리하기

}

template <class T>
int RecordTable<T>::Append(T& record) {//여기 구현을 하는 것이 시험 문제임+try,catch도 해야함
	string key = record.Key();
	int i = NumRecords;
	if (NumRecords == MaxKeys) {
		throw TableRecordsOverflowException();
		return 0;
	}
	//NumRecords에 있는 row records가 append할 수 있는지를 조사한다.
	if (data[i]->numKeys() == MaxRecords)//그 줄이 다 찼다는 내용
	{
		i = ++NumRecords;
	}
	try {//예외발생 못하면 점수 없을 예정
		int result = data[i]->Insert(key, record);
		return result;
	}
	catch (ExceptionError& error) {

		error.Print();
		return 0;

	}

	//return result;

}
//template <class T>
//int RecordTable<T>::Update(string key, const T& record) {
//   for (int i = 0; i < NumRecords; i++) {
//      int result = data[i].Find(key);//data[i]는 row records이고 여기에 key가 있는지를 조사
//      if (result > 0) {
//         data[i].Remove(key, Search(key));
//         data[i].Insert(key, record);
//         return 1;
//      }
//
//
//   }
//   return 0;
//}
template <class T>
int RecordTable<T>::Delete(string key) {//삭제하려는데 키가 없으면 예외발생을 시켜야 함
	if (NumRecords == 0) {
		throw EmptyException();
		return 0;
	}

	for (int i = 0; i < NumRecords + 1; i++) {
		try {
			return data[i]->Remove(key);

		}
		catch (ExceptionError& error) {
			error.Print();
		}
	}


}

template <class T>
void RecordTable<T>::Print(ostream& os) const {//텅 비어있을 때 출력하려고 하면 예외를 발생시켜야 함
	if (NumRecords == 0) {
		throw EmptyException();
		return;
	}

	for (int i = 0; i < NumRecords + 1; i++) {
		try {
			data[i]->Print(os);
		}
		catch (ExceptionError& error) {
			error.Print();
		}
	}
}

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
		cout << "\n선택 1: member  객체 30개 입력(Append()), 2.table 출력(Print()) , 3: table 객체 찾기 (Read()) 4: table에서 객체 삭제(Delete()), 5: 변경(update())" << endl;//()전부 다 RecordTable의 public 함수 들 
		cin >> select;
		switch (select) {
		case 1://table에 객체 30개 입력
		{
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


			for (int i = 0; i < 10; i++)
			{
				try {
					etable.Append(*members[i]);//overflow예외는 출력되어야함 table overflow는 main에서 해야하지만, 한 줄이 overflow면 메서드에서 함
				}
				catch (ExceptionError& error) {
					error.Print();
				}
			}

			cout << "2번째도 나와요" << endl;


			for (int i = 0; i < 20; i++)
			{
				try {
					wtable.Append(*workers[i]);//
				}
				catch (ExceptionError& error) {
					error.Print();
				}
			}



			break;
		}
		case 2:
			// table의 모든 객체 출력하기
			//table 모양으로 출력해야 한다: | *** | ???? | === |으로 출력한다.
			try {
				etable.Print(cout);
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			try {
				wtable.Print(cout);
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
		case 3:
			// table에서 객체 찾기
			try {
				e = &etable.Read("kim");



			}
			catch (ExceptionError& error) {
				error.Print();
			}
			try {
				ws = &wtable.Read("kim");
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
		case 4:
			//table에서 객체 삭제하기
			try {
				result = etable.Delete("hong");
				if (result > 0)
					cout << "삭제된 records 숫자" << result << endl;
			}
			catch (ExceptionError& error) {
				error.Print();
			}

			try {
				result = wtable.Delete("hong");
				if (result > 0)
					cout << "삭제된 records 숫자" << result << endl;
			}
			catch (ExceptionError& error) {
				error.Print();
			}
			break;
			//case 5:
			//   try {

			//      result = etable.Update("hong", *e);
			//      if (result > 0)
			//         cout << "삭제된 records 숫자" << result << endl;
			//      result = wtable.Update("hong", *ws);
			//   }
			//   catch () {
			//   }

			//   if (result > 0)
			//      cout << "삭제된 records 숫자" << result << endl;
			//   break;
		default:
			exit(0);

		}
	}

	system("pause");
	return 1;
}