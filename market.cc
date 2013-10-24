#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

struct Bid {
  Bid (double a, double p) : amount(a), price(p) {}
  double amount;
  double price; 

  double totalPrice () const {return amount * price;}
};

ostream& operator<< (ostream& os, const Bid& bid) {
  os << "(" << bid.amount << " at " << bid.price << ")";
  return os; 
}

struct BidSorter {
  bool ascend;
  BidSorter (bool a) : ascend(a) {}
  bool operator () (const Bid& one, const Bid& two) {
    if (ascend) return (one.price < two.price);
    return (one.price > two.price); 
  }
};

double findPrice (vector<Bid>& bids, vector<Bid>& asks, double& volume) {
  static BidSorter ascend(true);
  static BidSorter descend(false); 
  volume = 0; 
  if (0 == bids.size() * asks.size()) return -1; 

  sort(bids.begin(), bids.end(), descend);
  sort(asks.begin(), asks.end(), ascend); 

  if (bids[0].price < asks[0].price) return -1; // Highest bid lower than lowest asking price. 

  // Match buyers and sellers until one of three things happens:
  // - Run out of buyers
  // - Run out of sellers
  // - Current buyer and seller have incompatible offers
  // Price is set by last viable buyer/seller pair. 
  vector<Bid>::iterator currentBid = bids.begin(); 
  double price = (*currentBid).price; 
  vector<Bid>::iterator currentAsk = asks.begin(); 
  for (; currentBid != bids.end(); ++currentBid) {
    if ((*currentBid).price < (*currentAsk).price) break; // Incompatible 

    price = ((*currentBid).totalPrice() + (*currentAsk).totalPrice()) / ((*currentBid).amount + (*currentAsk).amount); 
    if ((*currentBid).amount <= (*currentAsk).amount) {
      // This buyer is satisfied - next!
      volume += (*currentBid).amount;
      (*currentAsk).amount -= (*currentBid).amount;
      continue; 
    }

    (*currentBid).amount -= (*currentAsk).amount; // This seller is done. 
    volume += (*currentAsk).amount;
    for (++currentAsk; currentAsk != asks.end(); ++currentAsk) {
      if ((*currentBid).price < (*currentAsk).price) break; // Incompatible 
      price = ((*currentBid).totalPrice() + (*currentAsk).totalPrice()) / ((*currentBid).amount + (*currentAsk).amount); 
      if ((*currentBid).amount <= (*currentAsk).amount) {
	volume += (*currentBid).amount;
	(*currentAsk).amount -= (*currentBid).amount;
	break; 
      }
      volume += (*currentAsk).amount;
      (*currentBid).amount -= (*currentAsk).amount; 
    }
    if (currentAsk == asks.end()) break; // Out of sellers. 
  }

  return price; 
}

struct Pop {
  Pop (double c, double l) : cash(c), labour(l) {}
  double cash;
  double labour; 
};


int main (int argc, char** argv) {

  Pop producer(1000, 0);
  Pop workers(1000, 0); 

  const double goodsPrice = 11; 

  // Store amount of goods needed to provide utility equal to
  // one unit of labour at different levels of goods. 
  vector<pair<double, double> > desiredGoods; 
  desiredGoods.push_back(pair<double, double>(20, 0.1));
  desiredGoods.push_back(pair<double, double>(20, 0.5));
  desiredGoods.push_back(pair<double, double>(20, 0.9));
  
  bool gotDesiredGoods = true; 
  double price = 10; 
  vector<Bid> bids;
  vector<Bid> asks; 
  for (int i = 0; i < 25; ++i) {
    bids.clear();
    asks.clear(); 

    // Producer splits capital into three parts, 
    // and won't deploy each part unless he makes
    // 10%, 20%, 30% profit. 
    bids.push_back(Bid(producer.cash*0.33/(goodsPrice*0.9), goodsPrice*0.9));
    bids.push_back(Bid(producer.cash*0.34/(goodsPrice*0.8), goodsPrice*0.8));
    bids.push_back(Bid(producer.cash*0.33/(goodsPrice*0.7), goodsPrice*0.7));

    for (unsigned int i = 0; i < desiredGoods.size(); ++i) {
      // Price at which I can buy X units of goods for one unit
      // of labour, where X is the amount of goods that gives the
      // same utility as that unit of labour. 
      double indifference = goodsPrice * desiredGoods[i].second; 

      // Offer to sell enough labour to buy Y units of goods, 
      // where Y is the amount that reaches my next margin. 
      asks.push_back(Bid(desiredGoods[i].first*goodsPrice/indifference, indifference)); 

      // Cash on hand should make a difference! 
    }

    cout << bids[0] << " "
	 << bids[1] << " "
	 << bids[2] << "\n"
	 << asks[0] << " "
	 << asks[1] << " "
	 << asks[2] << std::endl; 


    double volume = 0; 
    price = findPrice(bids, asks, volume);
    
    
    workers.cash  += volume*price;
    producer.cash -= volume*price;

    producer.cash += volume*goodsPrice;
    workers.cash -= min(workers.cash, (desiredGoods[0].first + desiredGoods[1].first + desiredGoods[2].first)*goodsPrice); 

    cout << volume << " at " << price << " giving " << (price*volume) << "; "
	 << producer.cash << ", " 
	 << workers.cash << ", " 
	 << producer.cash + workers.cash << endl; 
    
  }

  return 0; 
}
