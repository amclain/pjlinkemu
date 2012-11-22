/** 
 * \class Connection
 * 
 * \brief 
 */

#ifndef CONNECTION_H
#define	CONNECTION_H

class Connection {
public:
    Connection();
    ~Connection();

private:
    Connection(const Connection &orig);
    const Connection &operator=(const Connection &orig);
};

#endif	/* CONNECTION_H */

