#include "Particle.h"
//hi
Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition) : m_A(2, numPoints)
{
    vector<int> c(6); // 2 sets of color values in RGB format
    for (int i = 0; i < c.size(); i++) { c.at(i) = rand() % 256; }

    m_ttl = TTL;
    m_numPoints = numPoints;
    m_radiansPerSec = ((float)rand() / RAND_MAX) * M_PI; // Random radians per second
    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(static_cast<float>(target.getSize().x), -static_cast<float>(target.getSize().y));
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    m_vx = rand() % 401 + 100; // Range: [100, 500]
    m_vy = rand() % 401 + 100;

    m_color1 = Color(c[0], c[1], c[2], 255);
    m_color2 = Color(c[3], c[4], c[5], 255);

    float theta = ((float)rand() / RAND_MAX) * M_PI / 2.0f;
    float dTheta = 2 * M_PI / numPoints;

    for (int j = 0; j < numPoints; j++)
    {
        int r = rand() % 61 + 20; // Radius: [20, 80]
        int dx = static_cast<int>(r * cos(theta));
        int dy = static_cast<int>(r * sin(theta));

        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    VertexArray lines(TriangleFan, m_numPoints + 1);
    Vector2f center(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    lines[0].position = center;
    lines[0].color = m_color1;
    for (int j = 1; j <= m_numPoints; j++)
    {
        //target.mapCoordsToPixel(lines[j].position, m_cartesianPlane);
        Vector2f column_coordinate = sf::Vector2f(m_A(0, j - 1), m_A(1, j - 1));
        lines[j].position = sf::Vector2f(target.mapCoordsToPixel(column_coordinate, m_cartesianPlane));
        lines[j].color = m_color2;

    }
    target.draw(lines); // draw the vertexArray when the loop is finish
}


void Particle::update(float dt) {
    m_ttl -= dt; // Reduce time-to-live

    // Apply transformations
    rotate(dt * m_radiansPerSec);
    scale(SCALE);

    // Update position with velocity and gravity
    float dx = m_vx * dt;
    m_vy -= G * dt; // Apply gravity to y-velocity
    float dy = m_vy * dt;

    translate(dx, dy);
}

void Particle::rotate(double theta) {
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y); // Move to origin
    m_A = RotationMatrix(theta) * m_A;                      // Rotate
    translate(temp.x, temp.y);                              // Move back
}

void Particle::scale(double c) {
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y); // Move to origin
    m_A = ScalingMatrix(c) * m_A;                           // Scale
    translate(temp.x, temp.y);                              // Move back
}

void Particle::translate(double xShift, double yShift) {
    m_A = TranslationMatrix(xShift, yShift, m_A.getCols()) + m_A; // Translate vertices
    m_centerCoordinate.x += xShift;                              // Update center
    m_centerCoordinate.y += yShift;
}

bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}
void Particle::unitTests()
{
	int score = 0;
	cout << "Testing RotationMatrix constructor...";
	double theta = M_PI / 4.0;
	RotationMatrix r(M_PI / 4);
	if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
		&& almostEqual(r(0, 1), -sin(theta))
		&& almostEqual(r(1, 0), sin(theta))
		&& almostEqual(r(1, 1), cos(theta)))
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Testing ScalingMatrix constructor...";
	ScalingMatrix s(1.5);
	if (s.getRows() == 2 && s.getCols() == 2
		&& almostEqual(s(0, 0), 1.5)
		&& almostEqual(s(0, 1), 0)
		&& almostEqual(s(1, 0), 0)
		&& almostEqual(s(1, 1), 1.5))
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Testing TranslationMatrix constructor...";
	TranslationMatrix t(5, -5, 3);
	if (t.getRows() == 2 && t.getCols() == 3
		&& almostEqual(t(0, 0), 5)
		&& almostEqual(t(1, 0), -5)
		&& almostEqual(t(0, 1), 5)
		&& almostEqual(t(1, 1), -5)
		&& almostEqual(t(0, 2), 5)
		&& almostEqual(t(1, 2), -5))
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Testing Particles..." << endl;
	cout << "Testing Particle mapping to Cartesian origin..." << endl;
	if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
	{
		cout << "Failed. Expected (0,0). Received: (" << m_centerCoordinate.x <<
			"," << m_centerCoordinate.y << ")" << endl;
	}
	else
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	cout << "Applying one rotation of 90 degrees about the origin..." << endl;
	Matrix initialCoords = m_A;
	rotate(M_PI / 2.0);
	bool rotationPassed = true;
	for (int j = 0; j < initialCoords.getCols(); j++)
	{
		if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1,
			j), initialCoords(0, j)))
		{
			cout << "Failed mapping: ";
			cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
			rotationPassed = false;
		}
	}
	if (rotationPassed)
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Applying a scale of 0.5..." << endl;
	initialCoords = m_A;
	scale(0.5);
	bool scalePassed = true;
	for (int j = 0; j < initialCoords.getCols(); j++)
	{
		if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !
			almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
		{
			cout << "Failed mapping: ";
			cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
			scalePassed = false;
		}
	}
	if (scalePassed)
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Applying a translation of (10, 5)..." << endl;
	initialCoords = m_A;
	translate(10, 5);
	bool translatePassed = true;
	for (int j = 0; j < initialCoords.getCols(); j++)
	{
		if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !
			almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
		{
			cout << "Failed mapping: ";
			cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
			translatePassed = false;
		}
	}
	if (translatePassed)
	{
		cout << "Passed. +1" << endl;
		score++;
	}
	else
	{
		cout << "Failed." << endl;
	}
	cout << "Score: " << score << " / 7" << endl;
}
