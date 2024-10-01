// ignore_for_file: public_member_api_docs, sort_constructors_first
import 'dart:io';
import 'dart:math' as math;

List<Coordinate> coordinates = [];

void main() {
  try {
    print('\n');
    print('Reading file...');
    print('\n');
    readFile();

    Coordinate start = coordinates.first;
    Coordinate end = coordinates.last;

    print('Coordinates:');
    for (Coordinate coordinate in coordinates) {
      print(coordinate);
    }

    print('\n');

    print('Getting devations...');
    List<double> deviations = [];

    for (Coordinate coordinate in coordinates) {
      double devation = getDeviation(coordinate, start, end);
      deviations.add(devation);
    }
    print('\n');

    print('Deviations: ');

    for (double deviation in deviations) {
      print(deviation);
    }

    print('\n');

    print('Calculating RMSE...');
    print('\n');
    double rmse = calculateRMSE(deviations);
    print('RMSE = $rmse');
    print('\n');
  } catch (e) {
    print(e.toString());
  }
}

void readFile() {
  File file = File('input/data.csv');

  List<String> lines = file.readAsLinesSync();

  for (String line in lines) {
    List<String> tokens = line.split(',');

    double? lat = double.tryParse(tokens[1]);
    double? long = double.tryParse(tokens[2]);

    if (lat == null || long == null) throw Exception();

    Coordinate coordinate = Coordinate(lat, long);

    coordinates.add(coordinate);
  }
}

double getDeviation(Coordinate point, Coordinate start, Coordinate end) {
  final v1 = Coordinate(end.latitude - start.latitude, end.longitude - start.longitude);
  final v2 = Coordinate(point.latitude - start.latitude, point.longitude - start.longitude);
  final l2 = v1.longitude * v1.longitude + v1.latitude * v1.latitude;
  final projection = (v2.longitude * v1.longitude + v2.latitude * v1.latitude) / l2;

  if (projection > 0 && projection < 1) {
    final uy = start.latitude + v1.latitude * projection;
    final ux = start.longitude + v1.longitude * projection;
    return getDistance(point, Coordinate(uy, ux));
  } else {
    final d1 = getDistance(point, start);
    final d2 = getDistance(point, end);
    return math.min(d1, d2);
  }
}

double getDistance(Coordinate p1, Coordinate p2) {
  double latRad1 = p1.latitude * pi / 180;
  double latRad2 = p2.latitude * pi / 180;
  double latRadDiff = (p2.latitude - p1.latitude) * pi / 180;
  double lonRadDiff = (p2.longitude - p1.longitude) * pi / 180;

  double a = pow(sin(latRadDiff / 2), 2) + 
             pow(sin(lonRadDiff / 2), 2) *
             cos(latRad1) * cos(latRad2);

  return 2 * 6378137 * asin(sqrt(a));
}

class Coordinate {
  double latitude;
  double longitude;

  Coordinate(
    this.latitude,
    this.longitude,
  );

  @override
  String toString() => 'Coordinate(latitude: $latitude, longitude: $longitude)';
}

double calculateRMSE(List<double> deviations) {
  double squaredErrors = 0.0;

  for (int i = 0; i < deviations.length; i++) {
    double deviation = deviations[i];
    squaredErrors += deviation * deviation;
  }

  return math.sqrt(squaredErrors / deviations.length);
}
