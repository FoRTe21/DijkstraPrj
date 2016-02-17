#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define BUFSIZE 16							// 노드의 이름 저장 버퍼 크기
#define READSIZE 256						// text 파일 1줄 읽는 버퍼 크기
#define INF 99999							// Infinite 값

typedef struct Vertex						// 각 노드의 형식
{
	char m_nodeName[BUFSIZE];				// 노드의 이름
	int m_dist;								// 인접 노드와의 거리 
	
	struct Vertex *m_neighbor;				// 인접 노드를 가리키는 포인터
	struct Vertex *m_anotherVtx;			// 다른 vertex를 가리키는 포인터(인접 노드와는 무관)
} Vertex;

typedef struct VertexSet					// vertex 리스트.
{
	char m_nodeName[BUFSIZE];				// vertex 이름.
	int m_flag;								// 방문했는지에 대한 flag
} VertexSet;

Vertex* makeGraph(FILE *fp, int* num);		// 데이터 입력받은 후 adjacent list로 데이터 구조화.
int** makeArrayGraph(Vertex *head,VertexSet* set, int num);		// adjacency matrix로 데이터 구조화
VertexSet* makeNodesSet(Vertex* head, int num);			// Vertex의 list를 생성.
void InsertVertex(Vertex **head, char *firstName, char *secondName, int dist, int* num);//adjacent lit에 vertex 추가
void InsertNeighbor(Vertex *point, Vertex *nNode); // 인접 노드를 추가.

void dijkstra(int** array, VertexSet *sp, int num, char *startName, char *endName); // 다익스트라 알고리즘 적용 및 데이터 출력

void printGraph(Vertex *head);				// adjacent list화 되어 있는 자료 출력
void printNodesSet(VertexSet *sp, int num); // vertex list 출력
void printArrayGraph(int** array, int num); // adjacency matrix로 구조화 되어 있는 데이터 출력.  

time_t startTime = 0;
time_t endTime = 0;
float gap;

int main(int argc, char* argv[])
{

	if(argc == 1)
	{
		printf("type a name of input data\n");
		exit(1);
	}
	Vertex *head = NULL;						// adjacent list의 헤더 포인터
	VertexSet *sp = NULL;						// vertex list의 헤더 포인터
	int** array = NULL;							// adjacency matrix의 헤더 포인터
	int nbyn = 0;								// 총 vertex 개수
	FILE *fp = NULL;
	char start[BUFSIZE] = {0,};					// 출발 노드 이름 버퍼
	char end[BUFSIZE] = {0,};					// 도착 노드 이름 버퍼

	if((fp = fopen(argv[1], "rt")) == NULL)				// 입력 데이터가 있는 파일 열기
	//if((fp = fopen("testcase1.txt", "rt")) == NULL)
	{
		printf("fopen - error\n");
		exit(1);
	}
	head = makeGraph(fp, &nbyn);				// adjacent list 구성

	if(head == NULL)
	{
		printf("error - no data\n");
		exit(1);
	}
	printf("\nthe number of nodes : %d\n", nbyn);	// 데이터 출력
	printf("\nAdjacency List\n");
	printGraph(head);

	sp = makeNodesSet(head, nbyn);				// vertex list 구성

	printf("\nlist of nodes\n");				// vertex list 출력
	printNodesSet(sp, nbyn);	

	array =  makeArrayGraph(head, sp, nbyn);	// adjacency matrix 구성
	printf("\nAdjacency Matrix\n");				// adjacency matrix 출력
	printArrayGraph(array, nbyn);

	printf("input the start point : ");			// 출발 노드 이름 입력
	scanf("%s", start);

	printf("input the end point : ");			// 도착 노드 이름 입력
	scanf("%s", end);
	
	dijkstra(array, sp, nbyn, start, end);		// 다익스트라 알고리즘 적용 및 출력

	
	return 0;	
}

void dijkstra(int** array, VertexSet *sp, int num, char *startName, char *endName)
{
	int* dist = NULL;					// 출발노드로부터 각 노드까지의 shortest distance
	int i,j,k;
	int* via = NULL;					// 경유해야 하는 노드. path data
	int min = INF;
	int start;
	int end;	

	for(i = 0 ; i < num ; i++)			// vertex set에서 출발, 도착 노드의 index값 알아오기.
	{
		if(strcmp(sp[i].m_nodeName, startName) == 0)
		{
			start = i;
		}
		else if(strcmp(sp[i].m_nodeName, endName) == 0)
		{
			end = i;
		}
		
	}

	via = (int*)malloc(sizeof(int) * num);			
	dist = (int*)malloc(sizeof(int) * num);

	startTime = clock();
	for(i = 0; i < num; i++)					// 초기화
	{
		sp[i].m_flag = 0;						// 0 : 방문 안함, 1 : 방문함
		dist[i] = INF;							// 초기 각 노드로의 shortest distance는 INF
	}
	dist[start] = 0;							// 출발 노드는 shortest distance가 0.
	
	for(i = 0; i < num; i++)
	{
		min = INF;
		for(j = 0; j < num; j++)				// 방문하지 않은 노드이면서 shortest distance가 가장 작은 노드 검사.
		{
			if(sp[j].m_flag == 0 && dist[j] < min)
			{
				k = j;
				min = dist[j];
			}
		}	

		sp[k].m_flag = 1;					// 방문 flag를 1로 바꿈으로써 방문함을 표현.
		if(min == INF) break;				// min값이 INF로 바뀌지 않았다는 것은 주위에 노드가 없다는 의미. 

		for(j = 0; j < num; j++)			// 다익스트라 알고리즘의 핵심.
		{									// shortest distance(dist)가 중간 노드(k)를 거쳐가는
			if(sp[j].m_flag == 0 && dist[j] > dist[k] + array[k][j]) // dist[k] + array[k][j] 보다 크다면 
			{									// shortest distance의 값을 갱신.
				dist[j] = dist[k] + array[k][j];
				via[j] = k;					// 덤으로 갱신될 때의 거치는 node index 저장.
			}								// ex) j 노드를 갈 때 k 노드를 거치는 것이 짧다면 
		}									// via[j] 에 k 값을 저장.
	}

	endTime = clock();
	printf("%s ", sp[start].m_nodeName);

	int *path = (int*)malloc(sizeof(int)*num);
	int path_cnt = 0;

	k = end;

	while(1)
	{
		path[path_cnt++] = k;					// path값(via)을 순서대로 저장.
		if(k == start) break;					// via 값은 정렬이 되어 있지 않음.
		k = via[k];								// 단순이 via[k]는 k를 거친다는 데이터만 들어있음.
	}
	for(i = path_cnt - 2; i >= 0;i--)			// 정렬된 데이터를 이용하여 데이터 출력.
	{
		printf("-> (%d) -> %s ",array[path[i+1]][path[i]], sp[path[i]].m_nodeName);
	}
	printf("\n");
	printf("Shortest distance : %d\n", dist[end]); // 도착 node의 shortest distance 출력.

	gap = (float)(endTime-startTime)/(CLOCKS_PER_SEC);
	printf("spent time : %.100f\n", gap); 

}

void printNodesSet(VertexSet *sp, int num)
{
	int i = 0;
	for(i = 0; i < num; i++)
	{
		printf("%s ", sp[i].m_nodeName);
	} 
	printf("\n");
}

void printArrayGraph(int** array, int num)
{
	int i, j;
	for(i = 0; i < num; i++)
	{
		for(j = 0; j < num; j++)
		{
			if(array[i][j] == INF)
			{
				printf("INF ");
			}
			else
			{
				printf("%d ", array[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
}

void printGraph(Vertex *head)
{
	Vertex *p = head;
	Vertex *q;
	while(p != NULL)
	{
		q = p->m_neighbor;
		printf("%s -> ", p);
		while(q != NULL)
		{
			printf("(%d) -> %s -> ", q->m_dist, q->m_nodeName);
			q = q->m_neighbor;
		}	
		printf("\n");
		p = p->m_anotherVtx;
	}	
}

void InsertNeighbor(Vertex *point, Vertex *nNode)	// 인접 노드 삽입.
{
	int flag;
	Vertex *p = point->m_neighbor;
	Vertex *q = point;
	while(p != NULL)
	{
		flag = strcmp(p->m_nodeName, nNode->m_nodeName);
		if(flag == 0)						// 인접 노드 탐색 중 같은 이름이 있으면
		{									// 중복되었다는 의미이므로 node를 삭제 후 return.
			free(nNode);
			return ;
		}
		if(flag > 0)						// 정렬을 위해 뒤의 값보다 작으면 앞에 삽입.
		{
			q->m_neighbor = nNode;
			nNode->m_neighbor = p;
			return ;
		}
		else								// 탐색.
		{
			q = p;
			p = p->m_neighbor;
		}	
	}	
	q->m_neighbor = nNode;					// 탐색 실패시 마지막에 삽입.
}

void InsertVertex(Vertex **head, char *firstName, char *secondName, int dist, int* num)
{
	int flag;											// 새로운 노드 생성.
	Vertex *node = (Vertex*)malloc(sizeof(Vertex));
	strcpy(node->m_nodeName, secondName);
	node->m_dist = dist;
	node->m_neighbor = NULL;
	node->m_anotherVtx = NULL;


	if(*head == NULL)									// 최초의 값 노드 입력시.
	{
		(*num)++;	
		Vertex *fNode = (Vertex*)malloc(sizeof(Vertex));
		strcpy(fNode->m_nodeName, firstName);
		fNode->m_dist = 0;
		fNode->m_neighbor = node;
		fNode->m_anotherVtx = NULL;	
	
		*head = fNode;
		return;
	}	
	else											
	{
		Vertex *point = *head;	
		Vertex *q = NULL; 
		while(point != NULL)						// 탐색하면서 새로운 노드인지,
		{											// 인접 노드 삽입인지 확인 후 연산 수행.
			flag = strcmp(point->m_nodeName, firstName);
			if(flag == 0)
			{
				InsertNeighbor(point, node);	// 같은 이름의 노드가 있으면 
				return ;									// 인접 노드 삽입.
			}
			else if(flag > 0)								// 정렬을 위해 새로운 노드의 이름 값이
			{												// 뒤의 노드보다 작다면 앞에 삽입.
				(*num)++;
				Vertex *fNode = (Vertex*)malloc(sizeof(Vertex));
				strcpy(fNode->m_nodeName, firstName);
				fNode->m_dist = 0;
				fNode->m_neighbor = node;
				fNode->m_anotherVtx = NULL;
				if(q == NULL)								// head 노드보다 작을 때.
				{
					fNode->m_anotherVtx = point;
					*head = fNode;	
				}
				else 
				{
					fNode->m_anotherVtx = point;
					q->m_anotherVtx = fNode;
				}	
				return ;
			}
			else
			{
				q = point;	
				point = point->m_anotherVtx;	
			}	
		}													// 가장 뒤쪽에  삽입.
		(*num)++;
		Vertex *fNode = (Vertex*)malloc(sizeof(Vertex));
		strcpy(fNode->m_nodeName, firstName);
		fNode->m_dist = 0;
		fNode->m_neighbor = node;
		fNode->m_anotherVtx = NULL;

		q->m_anotherVtx = fNode;
			
	}
}

Vertex* makeGraph(FILE *fp, int* num)
{
	Vertex *head = NULL;
	char readBuffer[READSIZE] = {0,};
	char firstNodeName[BUFSIZE] = {0,};
	char secondNodeName[BUFSIZE] = {0,};
	int dist = 0;

	while(!feof(fp))
	{
		if(fgets(readBuffer, READSIZE, fp) <= 0)
		{
			break;
		}
		if(strlen(readBuffer) < 5)						// 들어오는 데이터는 적어도 크기 5 이상의 문자열. 노드 1, 2의 문자열은 적어도 1개의 문자 이상이고, 띄어쓰기 2, 거리 데이터는 1자리 이상.	
		{
			continue;	
		}
		strcpy(firstNodeName, strtok(readBuffer, " "));
		strcpy(secondNodeName, strtok(NULL," "));
		dist = atoi(strtok(NULL," "));
		InsertVertex(&head, firstNodeName, secondNodeName, dist, num);
		InsertVertex(&head, secondNodeName, firstNodeName, dist, num);
	}	 	
	

	fclose(fp);
	return head;
	
}

int** makeArrayGraph(Vertex *head, VertexSet *set, int num)
{
	//===================================================================
	//vertex list와 adjacent list를 이용하여 matrix 생성.
	//vertex list와 adjacent list는 정렬이 되어 있으므로
	//먼저 adjacent list의 노드를 확인하고
	//vertex list에서 adjacent list의 노드와 이름이 일치할 때까지
	//vertex list와 matrix의 index를 증가시키면서 INF를 입력.
	//vertex list에서의 이름과 adjacent list에서의 노드 이름이 같아지면
	//matrix에 관련 weight값을 입력해주고 
	//adjacent list의 포인터를 다음 노드로 이동하여 준다.
	//그 이후로 adjacent list의 포인터가 이동함에 따라 vertex list의 포인터도 이동.
	//함께 움직이면서 matrix의 값을 입력하여 준다.
	//adjacent list의 탐색이 다 끝났는데도 vertex list의 탐색이 다 끝나지 않았다면
	//나머지 남은 matrix의 값들은 INF로 채운다.
	//
	//ps. 나중에 보면 이해 못할 거 같다....
	//=================================================================================
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	int n = 0;
	int flag = 0;
	int** array = NULL;
	Vertex *p = NULL;
	Vertex *q = NULL;
	VertexSet *sp = NULL;

	p = head;
	sp = set;

	array = (int**)malloc(sizeof(int*)*num);
	for(i = 0; i < num; i++)
	{
		array[i] = (int*)malloc(sizeof(int) * num);
	}

	i = 0;
	while(p != NULL)
	{
		j = 0;
		q = p->m_neighbor;
		while(q != NULL)
		{
			if(i == j)						// 자신과 자신과의 거리값은 0
			{
				array[i][j] = 0;
				j++;
				continue;
			}	
			flag = strcmp(sp[j].m_nodeName, q->m_nodeName);	
			if(flag == 0)					// vertex list와 adjacent list에 있는 노드의 이름이 같으면
			{								// matrix값을 입력하고 index 증가.
				array[i][j] = q->m_dist;
				q = q->m_neighbor;
				j++;
			}	
			else 							// 같지 않으면 없다는 의미이므로 INF 값 저장.
			{
				array[i][j] = INF;	
				j++;
				continue;
			}
		}
		if(j < num)							// adjacent list는 탐색이 완료하였는데
		{									// matrix의 값이 완전히 입력이 되지 않았다면
			for(j;j<num;j++)				// 나머지 값들을 INF로 입력.
			{
				if(i == j)				
				{
					array[i][j] = 0;
					continue;
				}	
				array[i][j] = INF;
			}
		}
		i++;
		p = p->m_anotherVtx;					// 다음 vertex로 이동.
	}	

	return array;
}
VertexSet* makeNodesSet(Vertex* head, int num)
{
	int i = 0;
	VertexSet *set = NULL;
	Vertex *p = NULL;

	p = head;	
	
	set = (VertexSet*)malloc(sizeof(VertexSet) * num); 		// adjacent list를 확인하면서 vertex list 생성.
	for(i = 0; i < num; i++)
	{
		strcpy(set[i].m_nodeName, p->m_nodeName);
		set[i].m_flag = 0;
		p = p->m_anotherVtx;	
	}	

	return set;
}

