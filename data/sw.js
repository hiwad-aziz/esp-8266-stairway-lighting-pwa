self.addEventListener('install', function(event) {
  event.waitUntil(caches.open('sw-cache').then(function(cache) {
    return cache.addAll([
      'index.html', 'styles.css', 'images/stairs.png', 'images/stairs_icon.png',
      'src/app.js'
    ]);
  }));
});

self.addEventListener('fetch', function(event) {
  event.respondWith(caches.match(event.request).then(function(response) {
    return response || fetch(event.request);
  }));
});